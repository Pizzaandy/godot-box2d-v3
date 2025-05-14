#pragma once

#include "box2d_collision_object_2d.h"
#include "box2d_direct_body_state_2d.h"
#include <godot_cpp/templates/hash_set.hpp>

class Box2DDirectBodyState2D;

class Box2DBody2D final : public Box2DCollisionObject2D {
public:
	struct Contact {
		Box2DBody2D *body;
		float normal_impulse;
		Vector2 local_position;
		Vector2 local_normal;
		float depth = 0.0;
		int local_shape = 0;
		Vector2 collider_position;
		int collider_shape = 0;
		ObjectID collider_instance_id;
		RID collider;
		Vector2 collider_velocity;
		Vector2 impulse;
	};

	struct AreaOverrideAccumulator {
		Vector2 total_gravity = Vector2();
		bool skip_world_gravity = false;

		float total_linear_damp = 0.0;
		float total_angular_damp = 0.0;

		bool ignore_remaining_gravity = false;
		bool ignore_remaining_linear_damp = false;
		bool ignore_remaining_angular_damp = false;
	};

	Box2DBody2D();
	~Box2DBody2D();

	b2BodyId get_body_id() const { return body_id; }

	void set_bullet(bool p_bullet);
	bool get_bullet() const { return body_def.isBullet; }
	float get_bounce() const { return shape_def.material.restitution; }
	void set_bounce(float p_bounce);
	float get_friction() const { return shape_def.material.friction; }
	void set_friction(float p_friction);
	void reset_mass();
	float get_mass() const { return mass_data.mass; }
	float get_inverse_mass() const {
		float mass = get_mass();
		return mass > 0.0 ? 1.0 / mass : 0.0;
	}
	float get_inverse_inertia() const {
		float intertia = get_inertia();
		return intertia > 0.0 ? 1.0 / intertia : 0.0;
	}
	void set_mass(float p_mass);
	float get_inertia() const { return to_godot(to_godot(mass_data.rotationalInertia)); }
	void set_inertia(float p_inertia);
	Vector2 get_center_of_mass() const { return to_godot(mass_data.center); }
	Vector2 get_center_of_mass_global() const;
	void set_center_of_mass(const Vector2 &p_center);
	float get_gravity_scale() const { return body_def.gravityScale; }
	void set_gravity_scale(float p_scale);
	float get_linear_damping() const { return linear_damping; }
	void set_linear_damping(float p_damping);
	float get_angular_damping() const { return angular_damping; }
	void set_angular_damping(float p_damping);

	void update_linear_damping();
	void update_angular_damping();

	void apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position);
	void apply_impulse_global_point(const Vector2 &p_impulse, const Vector2 &p_position);
	void apply_impulse_center(const Vector2 &p_impulse);
	void apply_torque(float p_torque);
	void apply_torque_impulse(float p_impulse);
	void apply_force(const Vector2 &p_force, const Vector2 &p_position);
	void apply_central_force(const Vector2 &p_force);

	void set_linear_velocity(const Vector2 &p_velocity);
	Vector2 get_linear_velocity() const;
	Vector2 get_velocity_at_local_point(const Vector2 &p_point) const;
	Vector2 get_velocity_at_point(const Vector2 &p_point) const;
	void set_angular_velocity(float p_velocity);
	float get_angular_velocity() const;

	Vector2 get_static_linear_velocity() const { return static_linear_velocity; }
	float get_static_angular_velocity() const { return static_angular_velocity; }

	void set_sleep_state(bool p_sleeping);
	bool is_sleeping() { return sleeping; }
	void set_sleep_enabled(bool p_can_sleep);
	bool can_sleep() const { return body_def.enableSleep; }

	void sync_state(const b2Transform &p_transform, bool p_fell_asleep);

	void update_contacts();
	void get_contacts(int p_max_count = -1);

	int32_t get_contact_count();
	void set_max_contacts_reported(int32_t p_max_count) { max_contact_count = p_max_count; }
	int32_t get_max_contacts_reported() const { return max_contact_count; }
	void set_contact_depth_threshold(float p_threshold) { contact_depth_threshold = p_threshold; }
	float get_contact_depth_threshold() const { return contact_depth_threshold; }

	Vector2 get_contact_local_position(int p_contact_idx);
	Vector2 get_contact_local_normal(int p_contact_idx);
	int get_contact_local_shape(int p_contact_idx);
	RID get_contact_collider(int p_contact_idx);
	Vector2 get_contact_collider_position(int p_contact_idx);
	uint64_t get_contact_collider_id(int p_contact_idx);
	int get_contact_collider_shape(int p_contact_idx);
	Vector2 get_contact_impulse(int p_contact_idx);
	Vector2 get_contact_collider_velocity_at_position(int p_contact_idx);

	void add_collision_exception(RID p_rid);
	void remove_collision_exception(RID p_rid);
	TypedArray<RID> get_collision_exceptions() const;
	_FORCE_INLINE_ bool is_collision_exception(RID p_rid) const { return exceptions.has(p_rid); }

	float get_character_collision_priority() const { return character_collision_priority; }
	void set_character_collision_priority(float p_priority) { character_collision_priority = p_priority; }

	void set_shape_one_way_collision(int p_index, bool p_enabled, float p_margin);
	bool get_shape_one_way_collision(int p_index);

	Box2DDirectBodyState2D *get_direct_state();

	void set_state_sync_callback(const Callable &p_callable);
	void set_force_integration_callback(const Callable &p_callable, const Variant &p_user_data);

	void set_linear_damp_mode(PhysicsServer2D::BodyDampMode p_mode);
	PhysicsServer2D::BodyDampMode get_linear_damp_mode() const { return linear_damp_mode; }

	void set_angular_damp_mode(PhysicsServer2D::BodyDampMode p_mode);
	PhysicsServer2D::BodyDampMode get_angular_damp_mode() const { return angular_damp_mode; }

	void add_constant_force(const Vector2 &p_force, const Vector2 &p_position);
	void add_constant_central_force(const Vector2 &p_force);
	void add_constant_torque(float p_torque);
	void set_constant_force(const Vector2 &p_force);
	void set_constant_torque(float p_torque);

	Vector2 get_constant_force() const { return constant_force; }
	float get_constant_torque() const { return constant_torque; }

	void update_constant_forces_list();
	void apply_constant_forces();

	bool has_constant_forces() const {
		return !Math::is_zero_approx(constant_torque) || !constant_force.is_zero_approx();
	}
	bool has_static_velocity() const {
		return !Math::is_zero_approx(static_angular_velocity) || !static_linear_velocity.is_zero_approx();
	}

	void update_force_integration_list();
	void call_force_integration_callback();

	void update_mass();

	void apply_area_overrides();

	void shapes_changed() override;

	AreaOverrideAccumulator area_overrides;

protected:
	void on_added_to_space() override;
	void on_remove_from_space() override;

	uint64_t modify_mask_bits(uint32_t p_mask) override;

	Box2DDirectBodyState2D *direct_state = nullptr;

	HashSet<RID> exceptions;

	Vector2 constant_force = Vector2();
	float constant_torque = 0.0f;

	PhysicsServer2D::BodyDampMode linear_damp_mode = PhysicsServer2D::BODY_DAMP_MODE_COMBINE;
	PhysicsServer2D::BodyDampMode angular_damp_mode = PhysicsServer2D::BODY_DAMP_MODE_COMBINE;

	bool in_constant_forces_list = false;
	bool in_force_integration_list = false;
	bool sleeping = false;
	Vector2 initial_linear_velocity = Vector2();
	float initial_angular_velocity = 0.0f;

	bool use_static_velocities = false;
	Vector2 static_linear_velocity = Vector2();
	float static_angular_velocity = 0.0f;

	bool body_state_callback_is_valid = false;
	Callable body_state_callback;
	Callable force_integration_callback;
	Variant force_integration_user_data;

	float linear_damping = 0.0;
	float angular_damping = 0.0;

	float mass = 1.0f;
	b2MassData mass_data = b2MassData{ 0 };
	bool override_center_of_mass = false;
	Vector2 center_of_mass = Vector2();
	bool override_inertia = false;
	float inertia = 0.0f;

	float character_collision_priority = 0.0f;

	bool queried_contacts = false;
	int max_contact_count = 0;
	float contact_depth_threshold = -100.0f;
	// TODO: expose
	bool contact_ignore_speculative = true;
	LocalVector<Contact> contacts;
};
