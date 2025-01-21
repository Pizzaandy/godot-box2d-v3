#include "box2d_body_2d.h"
#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Box2DBody2D::Box2DBody2D() :
		Box2DCollisionObject2D(Type::RIGIDBODY) {
	if (Box2DProjectSettings::get_presolve_enabled()) {
		shape_def.enablePreSolveEvents = true;
	}
	shape_def.restitution = 0.0;
	shape_def.friction = 1.0;
	shape_def.filter.categoryBits = 1;
	shape_def.filter.maskBits = 1 | BODY_MASK_BIT;
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
	ERR_FAIL_COND(!body_exists);
	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyLinearImpulse(body_id, to_box2d(p_impulse), to_box2d(point), true);
}

void Box2DBody2D::apply_impulse_center(const Vector2 &p_impulse) {
	ERR_FAIL_COND(!body_exists);
	b2Body_ApplyLinearImpulseToCenter(body_id, to_box2d(p_impulse), true);
}

void Box2DBody2D::apply_torque(float p_torque) {
	ERR_FAIL_COND(!body_exists);
	b2Body_ApplyTorque(body_id, to_box2d(p_torque), true);
}

void Box2DBody2D::apply_torque_impulse(float p_impulse) {
	ERR_FAIL_COND(!body_exists);
	b2Body_ApplyAngularImpulse(body_id, to_box2d(p_impulse), true);
}

void Box2DBody2D::apply_force(const Vector2 &p_force, const Vector2 &p_position) {
	ERR_FAIL_COND(!body_exists);
	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyForce(body_id, to_box2d(p_force), to_box2d(point), true);
}

void Box2DBody2D::apply_central_force(const Vector2 &p_force) {
	ERR_FAIL_COND(!body_exists);
	b2Body_ApplyForceToCenter(body_id, to_box2d(p_force), true);
}

void Box2DBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	if (!body_exists) {
		initial_linear_velocity = p_velocity;
		return;
	}
	b2Body_SetLinearVelocity(body_id, to_box2d(p_velocity));
}

Vector2 Box2DBody2D::get_linear_velocity() const {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	return to_godot(b2Body_GetLinearVelocity(body_id));
}

Vector2 Box2DBody2D::get_velocity_at_local_point(const Vector2 &p_point) const {
	ERR_FAIL_COND_V(!body_exists, Vector2());

	Vector2 linear = to_godot(b2Body_GetLinearVelocity(body_id));
	float angular = b2Body_GetAngularVelocity(body_id);

	Vector2 relative_position = p_point - (get_transform().get_origin() + get_center_of_mass());

	return linear + Vector2(-angular * relative_position.y, angular * relative_position.x);
}

void Box2DBody2D::set_angular_velocity(float p_velocity) {
	if (!body_exists) {
		initial_angular_velocity = p_velocity;
		return;
	}
	b2Body_SetAngularVelocity(body_id, (float)p_velocity);
}

float Box2DBody2D::get_angular_velocity() const {
	ERR_FAIL_COND_V(!body_exists, 0.0);
	float angular_velocity = b2Body_GetAngularVelocity(body_id);
	return angular_velocity;
}

void Box2DBody2D::set_sleep_state(bool p_sleeping) {
	sleeping = p_sleeping;

	if (!body_exists) {
		// Sleep state can be set before the body is created.
		body_def.isAwake = !p_sleeping;
		return;
	}

	b2Body_SetAwake(body_id, !p_sleeping);
}

void Box2DBody2D::set_sleep_enabled(bool p_can_sleep) {
	body_def.enableSleep = p_can_sleep;

	if (!body_exists) {
		return;
	}

	b2Body_EnableSleep(body_id, p_can_sleep);
}

void Box2DBody2D::sync_state(const b2Transform &p_transform, bool p_fell_asleep) {
	if (!body_exists) {
		return;
	}

	queried_contacts = false;

	sleeping = p_fell_asleep;

	current_transform.set_origin(to_godot(p_transform.p));
	current_transform.set_rotation_scale_and_skew(
			b2Rot_GetAngle(p_transform.q),
			current_transform.get_scale(),
			current_transform.get_skew());

	if (body_state_callback.is_valid()) {
		TracyZoneScoped("Body State Callback");

		static thread_local Array arguments = []() {
			Array array;
			array.resize(1);
			return array;
		}();

		arguments[0] = get_direct_state();

		body_state_callback.callv(arguments);
	}
}

/// Lazily update contacts. This will only run when the user queries contact info once per step.
void Box2DBody2D::update_contacts() {
	if (!body_exists || queried_contacts) {
		return;
	}

	contacts.clear();

	if (max_contact_count <= 0) {
		return;
	}

	b2ContactData *data = memnew_arr(b2ContactData, max_contact_count);
	int contact_count = b2Body_GetContactData(body_id, data, max_contact_count);

	for (int i = 0; i < contact_count; i++) {
		b2ContactData box2d_contact = data[i];

		Box2DShapeInstance *local_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(box2d_contact.shapeIdA));
		Box2DShapeInstance *other_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(box2d_contact.shapeIdB));
		Box2DBody2D *other_body = static_cast<Box2DBody2D *>(b2Body_GetUserData(b2Shape_GetBody(box2d_contact.shapeIdB)));

		for (int point_index = 0; point_index < box2d_contact.manifold.pointCount; point_index++) {
			b2ManifoldPoint point = box2d_contact.manifold.points[point_index];

			float depth = -to_godot(point.separation);

			if (contact_ignore_speculative && point.maxNormalImpulse <= 0.0) {
				// contact is speculative
				continue;
			}

			if (depth < contact_depth_threshold) {
				continue;
			}

			Contact contact;
			contact.local_position = to_godot(point.point);
			contact.local_normal = to_godot(box2d_contact.manifold.normal).normalized();
			contact.depth = depth;
			contact.local_shape = local_shape->index;
			contact.collider_position = other_body->get_transform().get_origin();
			contact.collider_shape = other_shape->index;
			contact.collider_instance_id = other_body->get_instance_id();
			contact.collider = other_body->get_rid();
			contact.collider_velocity = other_body->get_velocity_at_local_point(to_godot(point.anchorB));

			Vector2 normal = contact.local_normal;
			Vector2 tangent = normal.orthogonal();
			contact.impulse = (normal * point.normalImpulse) + (tangent * point.tangentImpulse);

			contacts.push_back(contact);

			if (contacts.size() >= max_contact_count) {
				break;
			}
		}

		if (contacts.size() >= max_contact_count) {
			break;
		}
	}

	memdelete_arr(data);

	queried_contacts = true;
}

int32_t Box2DBody2D::get_contact_count() {
	ERR_FAIL_COND_V(!body_exists, 0);
	update_contacts();
	return contacts.size();
}

Vector2 Box2DBody2D::get_contact_local_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].local_position;
}

Vector2 Box2DBody2D::get_contact_local_normal(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].local_normal;
}

int Box2DBody2D::get_contact_local_shape(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].local_shape;
}

RID Box2DBody2D::get_contact_collider(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, RID());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), RID());
	return contacts[p_contact_idx].collider;
}

Vector2 Box2DBody2D::get_contact_collider_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].collider_position;
}

uint64_t Box2DBody2D::get_contact_collider_id(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].collider_instance_id;
}

int Box2DBody2D::get_contact_collider_shape(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].collider_shape;
}

Vector2 Box2DBody2D::get_contact_impulse(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].impulse;
}

Vector2 Box2DBody2D::get_contact_collider_velocity_at_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!body_exists, Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].collider_velocity;
}

void Box2DBody2D::add_collision_exception(RID p_rid) {
	exceptions.insert(p_rid);
}

void Box2DBody2D::remove_collision_exception(RID p_rid) {
	exceptions.erase(p_rid);
}

TypedArray<RID> Box2DBody2D::get_collision_exceptions() const {
	TypedArray<RID> result;
	result.resize(exceptions.size());

	int index = 0;
	for (RID rid : exceptions) {
		result[index] = rid;
		index++;
	}

	return result;
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

void Box2DBody2D::set_force_integration_callback(const Callable &p_callable, const Variant &p_user_data) {
	force_integration_callback = p_callable;
	force_integration_user_data = p_user_data;
	update_force_integration_list();
}

void Box2DBody2D::set_linear_damp_mode(PhysicsServer2D::BodyDampMode p_mode) {
	linear_damp_mode = p_mode;
}

void Box2DBody2D::set_angular_damp_mode(PhysicsServer2D::BodyDampMode p_mode) {
	angular_damp_mode = p_mode;
}

void Box2DBody2D::add_constant_force(const Vector2 &p_force, const Vector2 &p_position) {
	constant_force += p_force;

	// The torque applied from a constant force does not change if the center of mass is updated later.
	// This seems wrong, but it's consistent with Godot Physics
	constant_torque += (p_position - get_center_of_mass()).cross(p_force);

	update_constant_forces_list();
}

void Box2DBody2D::add_constant_central_force(const Vector2 &p_force) {
	constant_force += p_force;
	update_constant_forces_list();
}

void Box2DBody2D::add_constant_torque(float p_torque) {
	constant_torque += p_torque;
	update_constant_forces_list();
}

void Box2DBody2D::set_constant_force(const Vector2 &p_force) {
	constant_force = p_force;
	update_constant_forces_list();
}

void Box2DBody2D::set_constant_torque(float p_torque) {
	constant_torque = p_torque;
	update_constant_forces_list();
}

void Box2DBody2D::update_constant_forces_list() {
	if (!body_exists) {
		return;
	}

	ERR_FAIL_NULL(space);

	bool has_constant_forces = !Math::is_zero_approx(constant_torque) || !constant_force.is_zero_approx();

	if (has_constant_forces) {
		if (!in_constant_forces_list) {
			space->add_constant_force_body(this);
			in_constant_forces_list = true;
		}
	} else {
		if (in_constant_forces_list) {
			space->remove_constant_force_body(this);
			in_constant_forces_list = false;
		}
	}
}

void Box2DBody2D::apply_constant_forces() {
	ERR_FAIL_COND(!body_exists);

	if (!Math::is_zero_approx(constant_torque)) {
		apply_torque(constant_torque);
	}

	if (!constant_force.is_zero_approx()) {
		apply_central_force(constant_force);
	}
}

void Box2DBody2D::update_force_integration_list() {
	if (!body_exists) {
		return;
	}

	ERR_FAIL_NULL(space);

	if (force_integration_callback.is_valid()) {
		if (!in_force_integration_list) {
			space->add_force_integration_body(this);
			in_force_integration_list = true;
		}
	} else {
		if (in_force_integration_list) {
			space->remove_force_integration_body(this);
			in_force_integration_list = false;
		}
	}
}

void Box2DBody2D::call_force_integration_callback() {
	if (!body_exists) {
		return;
	}

	if (force_integration_callback.is_valid()) {
		if (force_integration_user_data.get_type() != Variant::NIL) {
			static thread_local Array arguments = []() {
				Array array;
				array.resize(2);
				return array;
			}();

			arguments[0] = get_direct_state();
			arguments[1] = force_integration_user_data;

			force_integration_callback.callv(arguments);
		} else {
			static thread_local Array arguments = []() {
				Array array;
				array.resize(1);
				return array;
			}();

			arguments[0] = get_direct_state();

			force_integration_callback.callv(arguments);
		}
	} else {
		ERR_FAIL_MSG("Invalid force integration callback");
	}
}

void Box2DBody2D::shapes_changed() {
	update_mass();
}

void Box2DBody2D::on_body_created() {
	// Bodies start asleep if their state is set with body_set_state before adding them to a space.
	// After creation, reset the sleep state to the default.
	body_def.isAwake = true;

	if (!initial_linear_velocity.is_zero_approx()) {
		b2Body_SetLinearVelocity(body_id, to_box2d(initial_linear_velocity));
		initial_linear_velocity = Vector2();
	}

	if (!Math::is_zero_approx(initial_angular_velocity)) {
		b2Body_SetAngularVelocity(body_id, to_box2d(initial_angular_velocity));
		initial_angular_velocity = 0.0f;
	}

	update_constant_forces_list();
	update_force_integration_list();
}

void Box2DBody2D::on_destroy_body() {
	if (in_constant_forces_list) {
		space->remove_constant_force_body(this);
		in_constant_forces_list = false;
	}

	if (in_force_integration_list) {
		space->remove_force_integration_body(this);
		in_force_integration_list = false;
	}
}

void Box2DBody2D::apply_area_overrides() {
	if (!body_exists || mode <= PhysicsServer2D::BODY_MODE_KINEMATIC) {
		return;
	}

	if (area_overrides.skip_world_gravity) {
		b2Body_SetGravityScale(body_id, 0.0f);
	} else {
		b2Body_SetGravityScale(body_id, body_def.gravityScale);
	}

	if (!area_overrides.total_gravity.is_zero_approx()) {
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

	area_overrides.total_gravity = Vector2();
	area_overrides.total_linear_damp = body_def.linearDamping;
	area_overrides.total_angular_damp = body_def.angularDamping;
}