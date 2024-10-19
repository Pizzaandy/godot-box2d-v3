#include "box2d_body_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Box2DBody2D::Box2DBody2D() {
	// read project settings here
}

Box2DBody2D::~Box2DBody2D() {
	if (direct_state) {
		memdelete(direct_state);
	}

	destroy_body();
}

void Box2DBody2D::destroy_body() {
	if (!body_exists) {
		return;
	}

	body_exists = false;

	if (b2Body_IsValid(body_id)) {
		b2DestroyBody(body_id);
	}

	body_id = b2_nullBodyId;
}

void Box2DBody2D::set_space(Box2DSpace2D *p_space) {
	destroy_body();

	if (space == p_space) {
		return;
	}

	space = p_space;

	if (!space) {
		return;
	}

	ERR_FAIL_COND(space->locked);

	// Create body
	body_def.position = to_box2d(current_transform.get_origin());
	body_def.rotation = b2MakeRot(current_transform.get_rotation());
	body_def.isBullet = is_bullet;

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

	body_id = b2CreateBody(space->get_world_id(), &body_def);
	b2Body_SetUserData(body_id, this);

	body_exists = true;

	rebuild_all_shapes();
}

void Box2DBody2D::set_mode(PhysicsServer2D::BodyMode p_mode) {
	if (!body_exists) {
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
			update_mass(false);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID_LINEAR:
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, true);
			update_mass(false);
			break;
	}

	if (mode == PhysicsServer2D::BodyMode::BODY_MODE_RIGID_LINEAR && p_mode != PhysicsServer2D::BodyMode::BODY_MODE_RIGID_LINEAR) {
		b2Body_SetFixedRotation(body_id, false);
		update_mass(false);
	}

	mode = p_mode;
}

void Box2DBody2D::set_bullet(bool p_bullet) {
	is_bullet = p_bullet;

	if (!body_exists) {
		return;
	}

	b2Body_SetBullet(body_id, p_bullet);
}

void Box2DBody2D::set_collision_layer(uint32_t p_layer) {
	shape_def.filter.categoryBits = p_layer;
	layer = p_layer;

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DBody2D::set_collision_mask(uint32_t p_mask) {
	shape_def.filter.maskBits = p_mask;
	mask = p_mask;

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DBody2D::set_transform(Transform2D p_transform, bool p_move_kinematic) {
	if (!body_exists) {
		current_transform = p_transform;
		return;
	}

	if (p_transform == current_transform) {
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
		current_transform = p_transform;
		rebuild_all_shapes();
	}

	current_transform = p_transform;
}

void Box2DBody2D::set_bounce(float p_bounce) {
	bounce = p_bounce;
	shape_def.restitution = p_bounce;

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetRestitution(id, shape_def.restitution);
	}
}

void Box2DBody2D::set_friction(float p_friction) {
	friction = p_friction;
	shape_def.friction = p_friction;

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFriction(id, shape_def.friction);
	}
}

void Box2DBody2D::reset_mass() {
	override_inertia = false;
	override_center_of_mass = false;

	if (!body_exists) {
		return;
	}

	update_mass(true);
}

void Box2DBody2D::set_mass(float p_mass) {
	mass = p_mass;

	if (!body_exists) {
		return;
	}

	update_mass(true);
}

void Box2DBody2D::set_inertia(float p_inertia) {
	inertia = p_inertia;
	override_inertia = true;

	if (!body_exists) {
		return;
	}

	update_mass(true);
}

void Box2DBody2D::set_center_of_mass(Vector2 p_center) {
	center_of_mass = p_center;
	override_center_of_mass = true;

	if (!body_exists) {
		return;
	}

	update_mass(true);
}

void Box2DBody2D::set_gravity_scale(float p_scale) {
	body_def.gravityScale = p_scale;

	if (!body_exists) {
		return;
	}

	b2Body_SetGravityScale(body_id, p_scale);
}

void Box2DBody2D::set_linear_damping(float p_damping) {
	body_def.linearDamping = p_damping;

	if (!body_exists) {
		return;
	}

	b2Body_SetLinearDamping(body_id, p_damping);
}

void Box2DBody2D::set_angular_damping(float p_damping) {
	body_def.angularDamping = p_damping;

	if (!body_exists) {
		return;
	}

	b2Body_SetAngularDamping(body_id, p_damping);
}

void Box2DBody2D::apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position) {
	ERR_FAIL_COND(is_locked());

	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyLinearImpulse(body_id, to_box2d(p_impulse), to_box2d(point), true);
}

void Box2DBody2D::apply_impulse_center(const Vector2 &p_impulse) {
	ERR_FAIL_COND(is_locked());

	b2Body_ApplyLinearImpulseToCenter(body_id, to_box2d(p_impulse), true);
}

void Box2DBody2D::apply_torque_impulse(float p_impulse) {
	ERR_FAIL_COND(is_locked());

	b2Body_ApplyAngularImpulse(body_id, p_impulse, true);
}

void Box2DBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	ERR_FAIL_COND(is_locked());

	b2Body_SetLinearVelocity(body_id, to_box2d(p_velocity));
}

Vector2 Box2DBody2D::get_linear_velocity() const {
	ERR_FAIL_COND_V(is_locked(), Vector2());

	return to_godot(b2Body_GetLinearVelocity(body_id));
}

void Box2DBody2D::set_angular_velocity(float p_velocity) {
	ERR_FAIL_COND(is_locked());

	b2Body_SetAngularVelocity(body_id, (float)p_velocity);
}

float Box2DBody2D::get_angular_velocity() const {
	ERR_FAIL_COND_V(is_locked(), 0.0);

	float angular_velocity = b2Body_GetAngularVelocity(body_id);
	return angular_velocity;
}

void Box2DBody2D::sync_state(b2Transform p_transform, bool fell_asleep) {
	current_transform.set_origin(to_godot(p_transform.p));
	current_transform.set_rotation(b2Rot_GetAngle(p_transform.q));
	sleeping = fell_asleep;

	if (body_state_callback.is_valid()) {
		static thread_local Array arguments = []() {
			Array array;
			array.resize(1);
			return array;
		}();

		arguments[0] = get_direct_state();

		body_state_callback.callv(arguments);
	}
}

RID Box2DBody2D::get_shape_rid(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), RID());
	Shape shape = shapes[p_index];
	return shape.shape->get_rid();
}

void Box2DBody2D::build_shape(Shape &p_shape, bool p_update_mass) {
	if (!body_exists) {
		return;
	}

	ERR_FAIL_COND(space->locked);

	Transform2D shape_transform = p_shape.transform.scaled(current_transform.get_scale());
	p_shape.build(body_id, shape_transform, shape_def);

	if (p_update_mass) {
		update_mass(false);
	}
}

void Box2DBody2D::rebuild_all_shapes() {
	for (Shape &shape : shapes) {
		build_shape(shape, false);
	}

	update_mass(false);
}

void Box2DBody2D::update_mass(bool p_recompute_from_shapes) {
	if (mode <= PhysicsServer2D::BODY_MODE_KINEMATIC) {
		return;
	}

	if (p_recompute_from_shapes) {
		b2Body_ApplyMassFromShapes(body_id);
	}

	mass_data = b2Body_GetMassData(body_id);
	mass_data.mass = mass;

	if (override_inertia) {
		mass_data.rotationalInertia = inertia;
	}
	if (override_center_of_mass) {
		mass_data.center = to_box2d(center_of_mass);
	}

	b2Body_SetMassData(body_id, mass_data);
}

void Box2DBody2D::add_shape(Box2DShape2D *p_shape, Transform2D p_transform, bool p_disabled) {
	Shape shape;
	shape.shape = p_shape;
	shape.transform = p_transform;
	shape.disabled = p_disabled;

	build_shape(shape, true);
	shapes.push_back(shape);

	int index = 0;
	for (Shape &shape : shapes) {
		shape.index = index;
		index++;
	}
}

void Box2DBody2D::set_shape(int p_index, Box2DShape2D *p_shape) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape &shape = shapes[p_index];
	shape.shape = p_shape;
	build_shape(shape, true);
}

void Box2DBody2D::remove_shape(int p_index) {
	ERR_FAIL_INDEX(p_index, shapes.size());

	Shape &shape = shapes[p_index];

	if (body_exists) {
		shape.destroy();
	}

	shapes.remove_at(p_index);

	int index = 0;
	for (Shape &shape : shapes) {
		shape.index = index;
		index++;
	}
}

void Box2DBody2D::set_shape_transform(int p_index, Transform2D p_transform) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape &shape = shapes[p_index];
	if (shape.transform == p_transform) {
		return;
	}
	shape.transform = p_transform;
	build_shape(shape, true);
}

Transform2D Box2DBody2D::get_shape_transform(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), Transform2D());
	Shape shape = shapes[p_index];
	return shape.transform;
}

Box2DDirectBodyState2D *Box2DBody2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DDirectBodyState2D(this));
	}
	return direct_state;
}