#include "box2d_body_2d.h"
#include "../joints/box2d_damped_spring_joint_2d.h"
#include "../joints/box2d_joint_2d.h"
#include "../spaces/box2d_space_2d.h"
#include "box2d_area_2d.h"
#include <godot_cpp/classes/animatable_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Box2DBody2D::Box2DBody2D() :
		Box2DCollisionObject2D(Type::RIGIDBODY) {
	if (Box2DProjectSettings::get_presolve_enabled()) {
		shape_def.enablePreSolveEvents = true;
	}
	shape_def.material.restitution = 0.0;
	shape_def.material.friction = 1.0;

	const uint32_t default_bitmask = 1u << 0;
	set_collision_layer(default_bitmask);
	set_collision_mask(default_bitmask);
}

Box2DBody2D::~Box2DBody2D() {
	if (direct_state) {
		memdelete(direct_state);
	}
}

void Box2DBody2D::set_bullet(bool p_bullet) {
	body_def.isBullet = p_bullet;

	if (!in_space()) {
		return;
	}

	b2Body_SetBullet(body_id, p_bullet);
}

void Box2DBody2D::set_bounce(float p_bounce) {
	shape_def.material.restitution = p_bounce;

	if (!in_space()) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetRestitution(id, shape_def.material.restitution);
	}
}

void Box2DBody2D::set_friction(float p_friction) {
	shape_def.material.friction = p_friction;

	if (!in_space()) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFriction(id, shape_def.material.friction);
	}
}

void Box2DBody2D::reset_mass() {
	override_inertia = false;
	override_center_of_mass = false;

	if (!in_space()) {
		return;
	}

	update_mass();
}

void Box2DBody2D::set_mass(float p_mass) {
	mass = p_mass;

	if (!in_space()) {
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

	if (!in_space()) {
		return;
	}

	update_mass();
}

Vector2 Box2DBody2D::get_center_of_mass_global() const {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	return to_godot(b2Body_GetWorldCenterOfMass(body_id));
}

void Box2DBody2D::set_center_of_mass(const Vector2 &p_center) {
	center_of_mass = p_center;
	override_center_of_mass = true;

	if (!in_space()) {
		return;
	}

	update_mass();
}

void Box2DBody2D::set_gravity_scale(float p_scale) {
	body_def.gravityScale = p_scale;

	if (!in_space()) {
		return;
	}

	b2Body_SetGravityScale(body_id, p_scale);
}

void Box2DBody2D::set_linear_damping(float p_damping) {
	linear_damping = p_damping;
	update_linear_damping();
}

void Box2DBody2D::set_angular_damping(float p_damping) {
	angular_damping = p_damping;
	update_angular_damping();
}

void Box2DBody2D::update_linear_damping() {
	if (!in_space()) {
		return;
	}

	body_def.linearDamping = linear_damping;

	if (linear_damp_mode == PhysicsServer2D::BODY_DAMP_MODE_COMBINE) {
		Box2DArea2D *default_area = space->get_default_area();
		ERR_FAIL_NULL(default_area);
		body_def.linearDamping += default_area->get_linear_damp();
	}

	b2Body_SetLinearDamping(body_id, body_def.linearDamping);
}

void Box2DBody2D::update_angular_damping() {
	if (!in_space()) {
		return;
	}

	body_def.angularDamping = angular_damping;

	if (angular_damp_mode == PhysicsServer2D::BODY_DAMP_MODE_COMBINE) {
		Box2DArea2D *default_area = space->get_default_area();
		ERR_FAIL_NULL(default_area);
		body_def.angularDamping += space->get_default_area()->get_angular_damp();
	}

	b2Body_SetAngularDamping(body_id, body_def.angularDamping);
}

void Box2DBody2D::apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position) {
	ERR_FAIL_COND(!in_space());
	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyLinearImpulse(body_id, to_box2d(p_impulse), to_box2d(point), true);
}

void Box2DBody2D::apply_impulse_global_point(const Vector2 &p_impulse, const Vector2 &p_position) {
	ERR_FAIL_COND(!in_space());
	b2Body_ApplyLinearImpulse(body_id, to_box2d(p_impulse), to_box2d(p_position), true);
}

void Box2DBody2D::apply_impulse_center(const Vector2 &p_impulse) {
	ERR_FAIL_COND(!in_space());
	b2Body_ApplyLinearImpulseToCenter(body_id, to_box2d(p_impulse), true);
}

void Box2DBody2D::apply_torque(float p_torque) {
	ERR_FAIL_COND(!in_space());
	b2Body_ApplyTorque(body_id, to_box2d(p_torque), true);
}

void Box2DBody2D::apply_torque_impulse(float p_impulse) {
	ERR_FAIL_COND(!in_space());
	b2Body_ApplyAngularImpulse(body_id, to_box2d(p_impulse), true);
}

void Box2DBody2D::apply_force(const Vector2 &p_force, const Vector2 &p_position) {
	ERR_FAIL_COND(!in_space());
	Vector2 point = current_transform.get_origin() + p_position;
	b2Body_ApplyForce(body_id, to_box2d(p_force), to_box2d(point), true);
}

void Box2DBody2D::apply_central_force(const Vector2 &p_force) {
	ERR_FAIL_COND(!in_space());
	b2Body_ApplyForceToCenter(body_id, to_box2d(p_force), true);
}

void Box2DBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	if (is_static()) {
		static_linear_velocity = p_velocity;
		update_constant_forces_list();
		return;
	}
	if (!in_space()) {
		initial_linear_velocity = p_velocity;
		return;
	}
	b2Body_SetLinearVelocity(body_id, to_box2d(p_velocity));
}

Vector2 Box2DBody2D::get_linear_velocity() const {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	return to_godot(b2Body_GetLinearVelocity(body_id));
}

Vector2 Box2DBody2D::get_velocity_at_local_point(const Vector2 &p_point) const {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	return get_velocity_at_point(current_transform.get_origin() + p_point);
}

Vector2 Box2DBody2D::get_velocity_at_point(const Vector2 &p_point) const {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	return to_godot(b2Body_GetWorldPointVelocity(body_id, to_box2d(p_point)));
}

void Box2DBody2D::set_angular_velocity(float p_velocity) {
	if (is_static()) {
		static_angular_velocity = p_velocity;
		update_constant_forces_list();
		return;
	}
	if (!in_space()) {
		initial_angular_velocity = p_velocity;
		return;
	}
	b2Body_SetAngularVelocity(body_id, (float)p_velocity);
}

float Box2DBody2D::get_angular_velocity() const {
	ERR_FAIL_COND_V(!in_space(), 0.0);
	float angular_velocity = b2Body_GetAngularVelocity(body_id);
	return angular_velocity;
}

void Box2DBody2D::set_sleep_state(bool p_sleeping) {
	sleeping = p_sleeping;

	if (!in_space()) {
		// Sleep state can be set before the body is created.
		body_def.isAwake = !p_sleeping;
		return;
	}

	b2Body_SetAwake(body_id, !p_sleeping);
}

void Box2DBody2D::set_sleep_enabled(bool p_can_sleep) {
	body_def.enableSleep = p_can_sleep;

	if (!in_space()) {
		return;
	}

	b2Body_EnableSleep(body_id, p_can_sleep);
}

/// Optimized function for updating rotation and position while preserving scale and skew.
/// The tradeoff is that values for scale and skew are subject to slight precision loss.
static void set_rotation_and_position_fast(Transform2D &p_xf, b2Rot p_rot, Vector2 p_pos) {
	TracyZoneScoped("Box2DBody2D::sync_state::set_rotation_and_position (fast)");

	b2Rot b = p_rot;
	b2Rot a = { p_xf.columns[0].x, p_xf.columns[0].y };

	float delta_angle = b2RelativeAngle(b, a);

	p_xf.columns[0] = p_xf.columns[0].rotated(delta_angle);
	p_xf.columns[1] = p_xf.columns[1].rotated(delta_angle);
	p_xf.columns[2] = p_pos;
}

/// Slower function for updating rotation and position. Consistent with Godot Physics.
static void set_rotation_and_position(Transform2D &p_xf, float p_rot, Vector2 p_pos) {
	TracyZoneScoped("Box2DBody2D::sync_state::set_rotation_and_position");

	real_t det_sign = Math::sign(p_xf.basis_determinant());
	Vector2 scale = Vector2(p_xf.columns[0].length(), det_sign * p_xf.columns[1].length());
	real_t skew = Math::acos(p_xf.columns[0].normalized().dot(det_sign * p_xf.columns[1].normalized())) - (real_t)Math_PI * 0.5f;

	p_xf = Transform2D(p_rot, scale, skew, p_pos);
}

void Box2DBody2D::sync_state(const b2Transform &p_transform, bool p_fell_asleep) {
	TracyZoneScoped("Box2DBody2D::sync_state");

	if (!in_space()) {
		return;
	}

	queried_contacts = false;
	sleeping = p_fell_asleep;

	set_rotation_and_position(current_transform, b2Rot_GetAngle(p_transform.q), to_godot(p_transform.p));

	if (body_state_callback_is_valid || body_state_callback.is_valid()) {
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
	if (!in_space() || queried_contacts) {
		return;
	}

	if (max_contact_count <= 0) {
		return;
	}

	get_contacts(max_contact_count);

	queried_contacts = true;
}

void Box2DBody2D::get_contacts(int p_max_count) {
	ERR_FAIL_COND(!in_space());

	contacts.clear();

	int manifold_count = b2Body_GetContactCapacity(body_id);

	if (p_max_count < 0) {
		p_max_count = manifold_count * 2;
	} else {
		p_max_count = Math::min(p_max_count, manifold_count * 2);
	}

	static LocalVector<b2ContactData> data;
	data.resize(manifold_count);
	int contact_count = b2Body_GetContactData(body_id, data.ptr(), manifold_count);

	for (int i = 0; i < contact_count; i++) {
		b2ContactData box2d_contact = data[i];

		Box2DShapeInstance *shape_a = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(box2d_contact.shapeIdA));
		Box2DShapeInstance *shape_b = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(box2d_contact.shapeIdB));

		bool owns_shape_a = shape_a->get_collision_object() == this;
		Box2DShapeInstance *local_shape = owns_shape_a ? shape_a : shape_b;
		Box2DShapeInstance *other_shape = owns_shape_a ? shape_b : shape_a;

		Box2DBody2D *other_body = other_shape->get_collision_object()->as_body();

		ERR_FAIL_NULL(other_body);

		for (int point_index = 0; point_index < box2d_contact.manifold.pointCount; point_index++) {
			b2ManifoldPoint point = box2d_contact.manifold.points[point_index];

			float depth = -to_godot(point.separation);

			if (contact_ignore_speculative && point.totalNormalImpulse <= 0.0) {
				// contact is speculative
				continue;
			}

			if (depth < contact_depth_threshold) {
				continue;
			}

			Contact contact;
			contact.body = other_body;
			contact.normal_impulse = to_godot(point.normalImpulse);
			contact.local_position = to_godot(point.point);
			contact.local_normal = to_godot_normalized(box2d_contact.manifold.normal);
			contact.depth = depth;
			contact.local_shape = local_shape->get_index();
			contact.collider_position = other_body->get_transform().get_origin();
			contact.collider_shape = other_shape->get_index();
			contact.collider_instance_id = other_body->get_instance_id();
			contact.collider = other_body->get_rid();
			contact.collider_velocity = other_body->get_velocity_at_local_point(to_godot(point.anchorB));

			Vector2 normal = contact.local_normal;
			Vector2 tangent = normal.orthogonal();
			contact.impulse = (normal * point.normalImpulse) + (tangent * point.tangentImpulse);

			contacts.push_back(contact);

			if (contacts.size() >= p_max_count) {
				return;
			}
		}

		if (contacts.size() >= p_max_count) {
			return;
		}
	}
}

int32_t Box2DBody2D::get_contact_count() {
	ERR_FAIL_COND_V(!in_space(), 0);
	update_contacts();
	return contacts.size();
}

Vector2 Box2DBody2D::get_contact_local_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].local_position;
}

Vector2 Box2DBody2D::get_contact_local_normal(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].local_normal;
}

int Box2DBody2D::get_contact_local_shape(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].local_shape;
}

RID Box2DBody2D::get_contact_collider(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), RID());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), RID());
	return contacts[p_contact_idx].collider;
}

Vector2 Box2DBody2D::get_contact_collider_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].collider_position;
}

uint64_t Box2DBody2D::get_contact_collider_id(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].collider_instance_id;
}

int Box2DBody2D::get_contact_collider_shape(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), -1);
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), -1);
	return contacts[p_contact_idx].collider_shape;
}

Vector2 Box2DBody2D::get_contact_impulse(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), Vector2());
	update_contacts();
	ERR_FAIL_INDEX_V(p_contact_idx, contacts.size(), Vector2());
	return contacts[p_contact_idx].impulse;
}

Vector2 Box2DBody2D::get_contact_collider_velocity_at_position(int p_contact_idx) {
	ERR_FAIL_COND_V(!in_space(), Vector2());
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
	Box2DShapeInstance &shape = shapes[p_index];
	shape.set_one_way_collision(p_one_way);
	shape.set_one_way_collision_margin(p_margin);
}

bool Box2DBody2D::get_shape_one_way_collision(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), false);
	Box2DShapeInstance &shape = shapes[p_index];
	return shape.has_one_way_collision();
}

void Box2DBody2D::update_mass() {
	if (!in_space()) {
		return;
	}

	BodyJointRange range(body_id);
	for (b2JointId id : range) {
		Box2DJoint2D *joint = static_cast<Box2DJoint2D *>(b2Joint_GetUserData(id));
		if (joint->get_type() == PhysicsServer2D::JOINT_TYPE_DAMPED_SPRING) {
			Box2DDampedSpringJoint2D *spring = static_cast<Box2DDampedSpringJoint2D *>(joint);
			spring->update_stiffness();
		}
	}

	if (!is_dynamic()) {
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

void Box2DBody2D::set_state_sync_callback(const Callable &p_callable) {
	body_state_callback = p_callable;
	Object *object = p_callable.get_object();

	// Questionable optimization: skip is_valid check when the state sync callback is bound to a RigidBody2D node
	RigidBody2D *rigidbody = Object::cast_to<RigidBody2D>(object);
	body_state_callback_is_valid = rigidbody && rigidbody->get_rid() == get_rid() && p_callable.is_custom();

	// HACK: always use kinematic movement when the state sync callback is bound to an AnimatableBody2D
	AnimatableBody2D *animatable_body = Object::cast_to<AnimatableBody2D>(object);
	is_animatable_body = animatable_body && animatable_body->get_rid() == get_rid();
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
	if (!in_space()) {
		return;
	}

	ERR_FAIL_NULL(space);

	if (has_constant_forces() || has_static_velocity()) {
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
	ERR_FAIL_COND(!in_space());

	if (!Math::is_zero_approx(constant_torque)) {
		apply_torque(constant_torque);
	}

	if (!constant_force.is_zero_approx()) {
		apply_central_force(constant_force);
	}

	if (has_static_velocity()) {
		get_contacts();

		for (Contact &contact : contacts) {
			Box2DBody2D *other = contact.body;
			if (!other->is_dynamic()) {
				continue;
			}

			Vector2 normal = contact.local_normal;
			Vector2 tangent = Vector2(-normal.y, normal.x);

			Vector2 r_a = contact.local_position - get_transform().get_origin();
			Vector2 r_b = contact.local_position - other->get_center_of_mass_global();

			Vector2 vel_other = other->get_velocity_at_point(contact.local_position);
			Vector2 vel_this = static_linear_velocity + Vector2(-r_a.y, r_a.x) * static_angular_velocity;
			Vector2 relative_velocity = vel_other - vel_this;

			float vrel_n = relative_velocity.dot(normal);
			float vrel_t = relative_velocity.dot(tangent);

			float desired_vrel_n = 0.0f;
			float desired_vrel_t = 0.0f;

			float delta_vn = desired_vrel_n - vrel_n;
			float delta_vt = desired_vrel_t - vrel_t;

			float inv_mass = other->get_inverse_mass();
			float inv_inertia = other->get_inverse_inertia();

			// Normal impulse
			float rn = r_b.cross(normal);
			float k_n = inv_mass + rn * rn * inv_inertia;
			float normal_impulse = (k_n > 0.0f) ? delta_vn / k_n : 0.0f;

			// Tangent impulse
			float rt = r_b.cross(tangent);
			float k_t = inv_mass + rt * rt * inv_inertia;
			float tangent_impulse = (k_t > 0.0f) ? delta_vt / k_t : 0.0f;

			// Apply total impulse
			Vector2 total_impulse = normal * normal_impulse + tangent * tangent_impulse;
			other->apply_impulse_global_point(total_impulse, contact.local_position);
		}
	}
}

void Box2DBody2D::update_force_integration_list() {
	if (!in_space()) {
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
	if (!in_space()) {
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

void Box2DBody2D::on_added_to_space() {
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

	update_linear_damping();
	update_angular_damping();
}

void Box2DBody2D::on_remove_from_space() {
	if (in_constant_forces_list) {
		space->remove_constant_force_body(this);
		in_constant_forces_list = false;
	}

	if (in_force_integration_list) {
		space->remove_force_integration_body(this);
		in_force_integration_list = false;
	}
}

uint64_t Box2DBody2D::modify_mask_bits(uint32_t p_mask) {
	uint64_t result = (uint64_t)p_mask | BODY_MASK_BIT;
	return result;
}

void Box2DBody2D::apply_area_overrides() {
	if (!in_space()) {
		return;
	}

	if (area_overrides.skip_world_gravity) {
		b2Body_SetGravityScale(body_id, 0.0);
	} else {
		b2Body_SetGravityScale(body_id, body_def.gravityScale);
	}

	if (!area_overrides.total_gravity.is_zero_approx()) {
		b2Body_ApplyForceToCenter(body_id, to_box2d(area_overrides.total_gravity), true);
	}

	if (linear_damp_mode == PhysicsServer2D::BODY_DAMP_MODE_COMBINE && area_overrides.total_linear_damp != body_def.linearDamping) {
		b2Body_SetLinearDamping(body_id, area_overrides.total_linear_damp);
	} else {
		b2Body_SetLinearDamping(body_id, body_def.linearDamping);
	}

	if (angular_damp_mode == PhysicsServer2D::BODY_DAMP_MODE_COMBINE && area_overrides.total_angular_damp != body_def.angularDamping) {
		b2Body_SetAngularDamping(body_id, area_overrides.total_angular_damp);
	} else {
		b2Body_SetAngularDamping(body_id, body_def.angularDamping);
	}

	area_overrides = AreaOverrideAccumulator();
	area_overrides.total_linear_damp = body_def.linearDamping;
	area_overrides.total_angular_damp = body_def.angularDamping;
}