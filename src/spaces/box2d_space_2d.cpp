#include "box2d_space_2d.h"

#include "../bodies/box2d_body_2d.h"
#include "../box2d_project_settings.h"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

struct Box2DTaskData {
	b2TaskCallback *task;
	int32_t item_count;
	int32_t task_count;
	void *task_context;
	WorkerThreadPool::GroupID group_id;
};

void group_task_function(void *p_userdata, uint32_t worker_index) {
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

	int32_t taskCount = CLAMP(itemCount / minRange, 1, space->max_tasks);

	Box2DTaskData *taskData = new Box2DTaskData{ task, itemCount, taskCount, taskContext };

	taskData->group_id = WorkerThreadPool::get_singleton()->add_native_group_task(group_task_function, taskData, taskCount, taskCount, false);

	return taskData;
}

void finish_task_callback(void *taskPtr, void *userContext) {
	if (taskPtr) {
		Box2DTaskData *taskData = static_cast<Box2DTaskData *>(taskPtr);
		WorkerThreadPool::get_singleton()->wait_for_group_task_completion(taskData->group_id);
		delete taskData;
	}
}

bool box2d_godot_presolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context) {
	Box2DBody2D *body_a = static_cast<Box2DBody2D *>(b2Body_GetUserData(b2Shape_GetBody(shapeIdA)));
	Box2DBody2D *body_b = static_cast<Box2DBody2D *>(b2Body_GetUserData(b2Shape_GetBody(shapeIdB)));

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
		Vector2 one_way_normal = (shape_a->transform * body_a->get_transform()).columns[1].normalized();
		Vector2 velocity = body_b->get_linear_velocity();
		float max_allowed_depth = velocity.length() * MAX(velocity.normalized().dot(one_way_normal), 0.0) + shape_a->one_way_collision_margin;
		if (contact_normal.dot(one_way_normal) <= 0.0 || depth > max_allowed_depth) {
			return false;
		}
	}

	if (shape_b->one_way_collision) {
		Vector2 one_way_normal = (shape_b->transform * body_b->get_transform()).columns[1].normalized();
		Vector2 velocity = body_a->get_linear_velocity();
		float max_allowed_depth = velocity.length() * MAX(velocity.normalized().dot(one_way_normal), 0.0) + shape_b->one_way_collision_margin;
		if (contact_normal.dot(one_way_normal) <= 0.0 || depth > max_allowed_depth) {
			return false;
		}
	}

	return true;
}

Box2DSpace2D::Box2DSpace2D() {
	substeps = Box2DProjectSettings::get_substeps();

	int hardware_thread_count = OS::get_singleton()->get_processor_count();
	int max_thread_count = Box2DProjectSettings::get_max_threads();

	if (max_thread_count < 1) {
		max_tasks = hardware_thread_count;
	} else {
		max_tasks = (max_thread_count < hardware_thread_count) ? max_thread_count : hardware_thread_count;
	}

	max_tasks = CLAMP(max_tasks, 1, 8);

	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = to_box2d(Box2DProjectSettings::get_default_gravity());

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

	ERR_FAIL_COND(!b2World_IsValid(world_id));

	b2DestroyWorld(world_id);
	world_id = b2_nullWorldId;
}

void Box2DSpace2D::step(float p_step) {
	locked = true;
	b2World_Step(world_id, p_step, substeps);
	locked = false;

	last_step = p_step;
}

void Box2DSpace2D::sync_state() {
	b2BodyEvents body_events = b2World_GetBodyEvents(world_id);

	for (int i = 0; i < body_events.moveCount; ++i) {
		const b2BodyMoveEvent *event = body_events.moveEvents + i;
		Box2DBody2D *body = static_cast<Box2DBody2D *>(event->userData);
		body->sync_state(event->transform, event->fellAsleep);
	}

	// b2SensorEvents sensor_events = b2World_GetSensorEvents(world_id);

	// for (int i = 0; i < sensor_events.beginCount; ++i) {
	// 	const b2SensorBeginTouchEvent *begin_event = sensor_events.beginEvents + i;
	// }

	// for (int i = 0; i < sensor_events.endCount; ++i) {
	// 	const b2SensorEndTouchEvent *end_event = sensor_events.endEvents + i;
	// }

	for (void *body : delete_after_sync) {
		memdelete(body);
	}

	delete_after_sync.clear();
}

Box2DDirectSpaceState2D *Box2DSpace2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DDirectSpaceState2D(this));
	}
	return direct_state;
}
