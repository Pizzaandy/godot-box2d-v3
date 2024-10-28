#include "box2d_body_2d.h"
#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Box2DBody2D::Box2DBody2D() {
	shape_def.enablePreSolveEvents = true;
}

Box2DBody2D::~Box2DBody2D() {
	if (direct_state) {
		memdelete(direct_state);
	}
}

void Box2DBody2D::set_bullet(bool p_bullet) {
	body_def.isBullet = p_bullet;

	if (!body_exists) {
		return;
	}

	b2Body_SetBullet(body_id, p_bullet);
}

void Box2DBody2D::set_bounce(float p_bounce) {
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

	update_mass();
}

void Box2DBody2D::set_mass(float p_mass) {
	mass = p_mass;

	if (!body_exists) {
		return;
	}

	update_mass();
}

void Box2DBody2D::set_inertia(float p_inertia) {
	if (p_inertia == 0.0) {
		override_inertia = false;
	} else {
		inertia = p_inertia;
		override_inertia = true;
	}

	if (!body_exists) {
		return;
	}

	update_mass();
}

void Box2DBody2D::set_center_of_mass(const Vector2 &p_center) {
	center_of_mass = p_center;
	override_center_of_mass = true;

	if (!body_exists) {
		return;
	}

	update_mass();
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

void Box2DBody2D::apply_torque(float p_torque) {
	ERR_FAIL_COND(is_locked());
	b2Body_ApplyTorque(body_id, p_torque, true);
}

void Box2DBody2D::apply_torque_impulse(float p_impulse) {
	ERR_FAIL_COND(is_locked());
	b2Body_ApplyAngularImpulse(body_id, p_impulse, true);
}

void Box2DBody2D::apply_force(const Vector2 &p_force, const Vector2 &p_position) {
	ERR_FAIL_COND(is_locked());
	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyForce(body_id, to_box2d(p_force), to_box2d(point), true);
}

void Box2DBody2D::apply_force_center(const Vector2 &p_force) {
	ERR_FAIL_COND(is_locked());
	b2Body_ApplyForceToCenter(body_id, to_box2d(p_force), true);
}

void Box2DBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	ERR_FAIL_COND(is_locked());
	b2Body_SetLinearVelocity(body_id, to_box2d(p_velocity));
}

Vector2 Box2DBody2D::get_linear_velocity() const {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	return to_godot(b2Body_GetLinearVelocity(body_id));
}

Vector2 Box2DBody2D::get_velocity_at_local_point(const Vector2 &p_point) const {
	ERR_FAIL_COND_V(is_locked(), Vector2());

	Vector2 linear = to_godot(b2Body_GetLinearVelocity(body_id));
	float angular = b2Body_GetAngularVelocity(body_id);

	Vector2 relative_position = p_point - (get_transform().get_origin() + get_center_of_mass());

	return linear + Vector2(-angular * relative_position.y, angular * relative_position.x);
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

void Box2DBody2D::sync_state(const b2Transform &p_transform, bool fell_asleep) {
	if (!body_exists) {
		return;
	}

	queried_contacts = false;

	sleeping = fell_asleep;
	current_transform.set_origin(to_godot(p_transform.p));
	current_transform.set_rotation_scale_and_skew(
			b2Rot_GetAngle(p_transform.q),
			current_transform.get_scale(),
			current_transform.get_skew());

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

void Box2DBody2D::update_contacts() {
	if (!body_exists || queried_contacts) {
		return;
	}

	delete[] contact_data;
	contact_count = b2Body_GetContactCapacity(body_id);
	contact_data = new b2ContactData[contact_count];
	b2Body_GetContactData(body_id, contact_data, contact_count);
	queried_contacts = true;
}

int32_t Box2DBody2D::get_contact_count() {
	ERR_FAIL_COND_V(!body_exists, 0);
	update_contacts();
	return contact_count;
}

void Box2DBody2D::add_collision_exception(RID p_rid) {
	exceptions.insert(p_rid);
}

void Box2DBody2D::remove_collision_exception(RID p_rid) {
	exceptions.erase(p_rid);
}

void Box2DBody2D::set_shape_one_way_collision(int p_index, bool p_one_way, float p_margin) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Box2DShapeInstance *shape = shapes[p_index];
	shape->one_way_collision = p_one_way;
	shape->one_way_collision_margin = p_margin;
}

bool Box2DBody2D::get_shape_one_way_collision(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), false);
	Box2DShapeInstance *shape = shapes[p_index];
	return shape->one_way_collision;
}

void Box2DBody2D::update_mass() {
	if (!body_exists || mode <= PhysicsServer2D::BODY_MODE_KINEMATIC) {
		return;
	}

	b2Body_ApplyMassFromShapes(body_id);

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

Box2DDirectBodyState2D *Box2DBody2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DDirectBodyState2D(this));
	}
	return direct_state;
}

void Box2DBody2D::set_linear_damp_mode(PhysicsServer2D::BodyDampMode p_mode) {
	linear_damp_mode = p_mode;
}

void Box2DBody2D::set_angular_damp_mode(PhysicsServer2D::BodyDampMode p_mode) {
	angular_damp_mode = p_mode;
}

void Box2DBody2D::shapes_changed() {
	update_mass();
}

void Box2DBody2D::apply_area_overrides() {
	if (!body_exists || mode <= PhysicsServer2D::BODY_MODE_KINEMATIC) {
		return;
	}

	if (area_overrides.total_gravity != Vector2()) {
		b2Body_ApplyForceToCenter(body_id, to_box2d(area_overrides.total_gravity), true);
	}

	if (area_overrides.total_linear_damp != body_def.linearDamping) {
		b2Body_SetLinearDamping(body_id, area_overrides.total_linear_damp);
	} else {
		b2Body_SetLinearDamping(body_id, body_def.linearDamping);
	}

	if (area_overrides.total_angular_damp != body_def.angularDamping) {
		b2Body_SetAngularDamping(body_id, area_overrides.total_angular_damp);
	} else {
		b2Body_SetLinearDamping(body_id, body_def.angularDamping);
	}

	area_overrides.total_gravity = space->get_default_gravity();
	area_overrides.total_linear_damp = body_def.linearDamping;
	area_overrides.total_angular_damp = body_def.angularDamping;
}

bool Box2DBody2D::is_locked() const {
	return !body_exists || !space || space->locked;
}