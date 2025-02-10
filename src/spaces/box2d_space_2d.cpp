#include "box2d_space_2d.h"

#include "../box2d_project_settings.h"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void group_task_function(void *p_userdata, uint32_t worker_index) {
	TracyZoneScoped("Box2D Group Task");

	Box2DTaskData *data = static_cast<Box2DTaskData *>(p_userdata);

	int32_t items_per_task = data->item_count / data->task_count;

	int32_t startIndex = worker_index * items_per_task;
	int32_t endIndex = startIndex + items_per_task;
	if (worker_index == data->task_count - 1) {
		endIndex = data->item_count;
	}

	data->task(startIndex, endIndex, worker_index, data->task_context);
}

void *enqueue_task_callback(b2TaskCallback *task, int32_t itemCount, int32_t minRange, void *taskContext, void *userContext) {
	Box2DSpace2D *space = static_cast<Box2DSpace2D *>(userContext);

	int32_t task_count = CLAMP(itemCount / minRange, 1, space->get_max_tasks());

	Box2DTaskData *task_data = new Box2DTaskData{ 0, taskContext, task, itemCount, task_count };

	task_data->group_id = WorkerThreadPool::get_singleton()->add_native_group_task(group_task_function, task_data, task_count, task_count, true);

	return task_data;
}

void finish_task_callback(void *taskPtr, void *userContext) {
	if (taskPtr) {
		Box2DTaskData *task_data = static_cast<Box2DTaskData *>(taskPtr);
		WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_data->group_id);
		delete task_data;
	}
}

bool box2d_godot_presolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context) {
	Box2DCollisionObject2D *object_a = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(shapeIdA)));
	Box2DCollisionObject2D *object_b = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(shapeIdB)));

	if (object_a->is_area() || object_b->is_area()) {
		return true;
	}

	Box2DBody2D *body_a = static_cast<Box2DBody2D *>(object_a);
	Box2DBody2D *body_b = static_cast<Box2DBody2D *>(object_b);

	if (body_a->is_collision_exception(body_b->get_rid()) ||
			body_b->is_collision_exception(body_a->get_rid())) {
		return false;
	}

	float depth;

	if (manifold->pointCount == 2) {
		depth = -to_godot(MIN(manifold->points[0].separation, manifold->points[1].separation));
	} else if (manifold->pointCount == 1) {
		depth = -to_godot(manifold->points[0].separation);
	} else {
		return false;
	}

	Box2DShapeInstance *shape_a = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeIdA));
	Box2DShapeInstance *shape_b = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeIdB));

	Vector2 contact_normal = to_godot(manifold->normal).normalized();

	if (shape_a->one_way_collision) {
		Vector2 one_way_normal = -(shape_a->transform * body_a->get_transform()).columns[1].normalized();
		Vector2 velocity = body_b->get_linear_velocity();
		float max_allowed_depth = velocity.length() * MAX(velocity.normalized().dot(one_way_normal), 0.0) + shape_a->one_way_collision_margin;
		if (contact_normal.dot(one_way_normal) <= 0.0 || depth > max_allowed_depth) {
			return false;
		}
	}

	if (shape_b->one_way_collision) {
		Vector2 one_way_normal = -(shape_b->transform * body_b->get_transform()).columns[1].normalized();
		Vector2 velocity = body_a->get_linear_velocity();
		float max_allowed_depth = velocity.length() * MAX(velocity.normalized().dot(one_way_normal), 0.0) + shape_b->one_way_collision_margin;
		if (contact_normal.dot(one_way_normal) <= 0.0 || depth > max_allowed_depth) {
			return false;
		}
	}

	return true;
}

float friction_callback(float frictionA, int materialA, float frictionB, int materialB) {
	return ABS(MIN(frictionA, frictionB));
}

float restitution_callback(float restitutionA, int materialA, float restitutionB, int materialB) {
	return CLAMP(restitutionA + restitutionB, 0.0f, 1.0f);
}

Box2DSpace2D::Box2DSpace2D() {
	substeps = Box2DProjectSettings::get_substeps();

	// Gravity is changed by the default area immediately - the value set here doesn't matter.
	default_gravity = Vector2(0.0, 9.8);

	int hardware_thread_count = OS::get_singleton()->get_processor_count();
	int max_thread_count = Box2DProjectSettings::get_max_threads();

	if (max_thread_count < 1) {
		max_tasks = hardware_thread_count;
	} else {
		max_tasks = (max_thread_count < hardware_thread_count) ? max_thread_count : hardware_thread_count;
	}

	max_tasks = CLAMP(max_tasks, 1, 8);

	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = to_box2d(default_gravity);

	// TODO: add settings for mixing rules
	world_def.frictionCallback = friction_callback;
	world_def.restitutionCallback = restitution_callback;

	world_def.workerCount = max_tasks;
	world_def.userTaskContext = this;
	world_def.enqueueTask = enqueue_task_callback;
	world_def.finishTask = finish_task_callback;

	world_id = b2CreateWorld(&world_def);

	if (Box2DProjectSettings::get_presolve_enabled()) {
		b2World_SetPreSolveCallback(world_id, box2d_godot_presolve, nullptr);
	}
}

Box2DSpace2D::~Box2DSpace2D() {
	if (direct_state) {
		memdelete(direct_state);
	}

	if (b2World_IsValid(world_id)) {
		b2DestroyWorld(world_id);
	}

	world_id = b2_nullWorldId;
}

void Box2DSpace2D::step(float p_step) {
	locked = true;

	for (Box2DBody2D *body : constant_force_list) {
		body->apply_constant_forces();
	}

	for (Box2DArea2D *area : areas_to_step) {
		area->step();
	}

	b2World_Step(world_id, p_step, substeps);
	locked = false;

	last_step = p_step;
}

void Box2DSpace2D::sync_state() {
	for (Box2DBody2D *body : force_integration_list) {
		body->call_force_integration_callback();
	}

	b2BodyEvents body_events = b2World_GetBodyEvents(world_id);

	for (int i = 0; i < body_events.moveCount; ++i) {
		const b2BodyMoveEvent *event = body_events.moveEvents + i;
		Box2DCollisionObject2D *object = static_cast<Box2DCollisionObject2D *>(event->userData);
		if (!object->is_rigidbody()) {
			continue;
		}
		Box2DBody2D *body = static_cast<Box2DBody2D *>(object);
		body->sync_state(event->transform, event->fellAsleep);
	}

	b2SensorEvents sensor_events = b2World_GetSensorEvents(world_id);

	for (int i = 0; i < sensor_events.beginCount; ++i) {
		const b2SensorBeginTouchEvent *begin_event = sensor_events.beginEvents + i;
		if (!b2Shape_IsValid(begin_event->sensorShapeId) || !b2Shape_IsValid(begin_event->visitorShapeId)) {
			continue;
		}

		Box2DShapeInstance *self_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(begin_event->sensorShapeId));
		Box2DCollisionObject2D *self_object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(begin_event->sensorShapeId)));
		ERR_CONTINUE_MSG(!self_object->is_area(), "Received a sensor event from a Rigidbody. This should never happen!");
		Box2DArea2D *area = static_cast<Box2DArea2D *>(self_object);

		Box2DShapeInstance *other_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(begin_event->visitorShapeId));
		Box2DCollisionObject2D *other_object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(begin_event->visitorShapeId)));

		area->report_event(
				other_object->get_type(),
				PhysicsServer2D::AREA_BODY_ADDED,
				other_object->get_rid(),
				other_object->get_instance_id(),
				other_shape->index,
				self_shape->index);
	}

	for (int i = 0; i < sensor_events.endCount; ++i) {
		const b2SensorEndTouchEvent *end_event = sensor_events.endEvents + i;
		if (!b2Shape_IsValid(end_event->sensorShapeId) || !b2Shape_IsValid(end_event->visitorShapeId)) {
			continue;
		}

		Box2DShapeInstance *self_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(end_event->sensorShapeId));
		Box2DCollisionObject2D *self_object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(end_event->sensorShapeId)));
		ERR_CONTINUE_MSG(!self_object->is_area(), "Received a sensor event from a Rigidbody. This should never happen!");

		Box2DArea2D *area = static_cast<Box2DArea2D *>(self_object);

		Box2DShapeInstance *other_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(end_event->visitorShapeId));
		Box2DCollisionObject2D *other_object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(b2Shape_GetBody(end_event->visitorShapeId)));

		area->report_event(
				other_object->get_type(),
				PhysicsServer2D::AREA_BODY_REMOVED,
				other_object->get_rid(),
				other_object->get_instance_id(),
				other_shape->index,
				self_shape->index);
	}
}

Box2DDirectSpaceState2D *Box2DSpace2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DDirectSpaceState2D(this));
	}
	return direct_state;
}

void Box2DSpace2D::set_default_gravity(Vector2 p_gravity) {
	default_gravity = p_gravity;
	b2World_SetGravity(world_id, to_box2d(default_gravity));
}