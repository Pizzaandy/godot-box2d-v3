#include "box2d_body_2d.h"
#include "../type_conversions.h"
#include <godot_cpp/variant/utility_functions.hpp>

Box2DBody2D::~Box2DBody2D() {
	if (b2Body_IsValid(body_id)) {
		b2DestroyBody(body_id);
		body_id = b2_nullBodyId;
	}
	if (direct_state) {
		memdelete(direct_state);
	}
}

void Box2DBody2D::set_space(Box2DSpace2D *p_space) {
	if (B2_IS_NON_NULL(body_id)) {
		b2DestroyBody(body_id);
		body_id = b2_nullBodyId;
	}

	if (p_space != nullptr) {
		// Create body
		body_def.position = to_box2d(current_transform.get_origin());
		body_def.rotation = b2MakeRot(current_transform.get_rotation());

		switch (mode) {
			case PhysicsServer2D::BODY_MODE_STATIC:
				body_def.type = b2BodyType::b2_staticBody;
				break;
			case PhysicsServer2D::BODY_MODE_KINEMATIC:
				body_def.type = b2BodyType::b2_kinematicBody;
				break;
			case PhysicsServer2D::BODY_MODE_RIGID:
				body_def.type = b2BodyType::b2_dynamicBody;
				break;
			case PhysicsServer2D::BODY_MODE_RIGID_LINEAR:
				body_def.type = b2BodyType::b2_dynamicBody;
				body_def.fixedRotation = true;
				break;
		}

		body_id = b2CreateBody(p_space->get_world_id(), &body_def);
		b2Body_SetUserData(body_id, this);
		for (Shape &shape : shapes) {
			build_shape(shape);
		}
	}

	space = p_space;
}

Box2DSpace2D *Box2DBody2D::get_space() {
	return space;
}

void Box2DBody2D::set_mode(PhysicsServer2D::BodyMode p_mode) {
	if (B2_IS_NULL(body_id)) {
		mode = p_mode;
		return;
	}

	switch (p_mode) {
		case PhysicsServer2D::BODY_MODE_STATIC:
			b2Body_SetType(body_id, b2BodyType::b2_staticBody);
			break;
		case PhysicsServer2D::BODY_MODE_KINEMATIC:
			b2Body_SetType(body_id, b2BodyType::b2_kinematicBody);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID:
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID_LINEAR:
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, true);
			break;
	}

	if (mode == PhysicsServer2D::BodyMode::BODY_MODE_RIGID_LINEAR && p_mode != PhysicsServer2D::BodyMode::BODY_MODE_RIGID_LINEAR) {
		b2Body_SetFixedRotation(body_id, false);
	}

	mode = p_mode;
}

PhysicsServer2D::BodyMode Box2DBody2D::get_mode() {
	return mode;
}

void Box2DBody2D::set_transform(Transform2D p_transform, bool p_move_kinematic) {
	if (B2_IS_NULL(body_id)) {
		current_transform = p_transform;
		return;
	}

	Vector2 position = p_transform.get_origin();
	float rotation = p_transform.get_rotation();
	float last_step = space->get_last_step();

	if (p_move_kinematic && mode == PhysicsServer2D::BODY_MODE_KINEMATIC) {
		if (last_step < 0.0) {
			current_transform = p_transform;
			return;
		}

		Vector2 current_position = to_godot(b2Body_GetPosition(body_id));
		Vector2 linear = (position - current_position) / last_step;

		b2Rot target_rotation = b2MakeRot(rotation);
		b2Rot current_rotation = b2MakeRot(current_transform.get_rotation());
		float angular = b2RelativeAngle(target_rotation, current_rotation) / last_step;

		b2Body_SetLinearVelocity(body_id, to_box2d(linear));
		b2Body_SetAngularVelocity(body_id, (float)angular);
	} else {
		b2Body_SetTransform(body_id, to_box2d(position), b2MakeRot(rotation));
		b2Body_SetAwake(body_id, true);
	}

	if (current_transform.get_scale() != p_transform.get_scale()) {
		for (Shape &shape : shapes) {
			build_shape(shape);
		}
	}

	current_transform = p_transform;
}

Transform2D Box2DBody2D::get_transform() {
	return current_transform;
}

void Box2DBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	ERR_FAIL_COND(B2_IS_NULL(body_id));
	b2Body_SetLinearVelocity(body_id, to_box2d(p_velocity));
}

Vector2 Box2DBody2D::get_linear_velocity() const {
	ERR_FAIL_COND_V(B2_IS_NULL(body_id), Vector2());
	b2Vec2 velocity = b2Body_GetLinearVelocity(body_id);
	return to_godot(velocity);
}

void Box2DBody2D::set_angular_velocity(float p_velocity) {
	ERR_FAIL_COND(B2_IS_NULL(body_id));
	b2Body_SetAngularVelocity(body_id, (float)p_velocity);
}

float Box2DBody2D::get_angular_velocity() const {
	ERR_FAIL_COND_V(B2_IS_NULL(body_id), 0.0);
	float angular_velocity = b2Body_GetAngularVelocity(body_id);
	return angular_velocity;
}

void Box2DBody2D::sync_state(b2Transform p_transform, bool fell_asleep) {
	current_transform.set_origin(to_godot(p_transform.p));
	current_transform.set_rotation(b2Rot_GetAngle(p_transform.q));
	sleeping = fell_asleep;

	if (body_state_callback.is_valid()) {
		static thread_local Array arg_array = []() {
			Array array;
			array.resize(1);
			return array;
		}();

		arg_array[0] = get_direct_state();

		body_state_callback.callv(arg_array);
	}
}

RID Box2DBody2D::get_shape_rid(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), RID());
	Shape shape = shapes[p_index];
	return shape.shape->get_rid();
}

void Box2DBody2D::build_shape(Shape &p_shape) {
	if (B2_IS_NULL(body_id)) {
		return;
	}

	b2ShapeDef shape_def = b2DefaultShapeDef();
	Transform2D shape_transform = p_shape.transform.scaled(current_transform.get_scale());
	p_shape.build(body_id, shape_transform, shape_def);
}

void Box2DBody2D::add_shape(Box2DShape2D *p_shape, Transform2D p_transform, bool p_disabled) {
	Box2DBody2D::Shape shape;
	shape.shape = p_shape;
	shape.transform = p_transform;
	shape.disabled = p_disabled;

	build_shape(shape);
	shapes.push_back(shape);
}

void Box2DBody2D::set_shape(int p_index, Box2DShape2D *p_shape) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape &shape = shapes[p_index];
	shape.shape = p_shape;
	build_shape(shape);
}

void Box2DBody2D::remove_shape(int p_index) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape &shape = shapes[p_index];
	shape.destroy();

	shapes.remove_at(p_index);
}

void Box2DBody2D::set_shape_transform(int p_index, Transform2D p_transform) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape &shape = shapes[p_index];
	shape.transform = p_transform;
	build_shape(shape);
}

Transform2D Box2DBody2D::get_shape_transform(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), Transform2D());
	Shape shape = shapes[p_index];
	return shape.transform;
}

int32_t Box2DBody2D::get_shape_count() {
	return shapes.size();
}

Box2DDirectBodyState2D *Box2DBody2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DDirectBodyState2D(this));
	}
	return direct_state;
}

void Box2DBody2D::set_state_sync_callback(const Callable &p_callable) {
	body_state_callback = p_callable;
}