#pragma once

#include "box2d_body_2d.h"
#include <godot_cpp/classes/physics_direct_body_state2d.hpp>
#include <godot_cpp/classes/physics_direct_body_state2d_extension.hpp>
#include <godot_cpp/classes/physics_server2d.hpp>

using namespace godot;

class Box2DBody2D;

class Box2DDirectBodyState2D : public PhysicsDirectBodyState2DExtension {
	GDCLASS(Box2DDirectBodyState2D, PhysicsDirectBodyState2DExtension);

	Box2DBody2D *body = nullptr;

protected:
	static void _bind_methods() {}

public:
	Box2DDirectBodyState2D() = default;
	explicit Box2DDirectBodyState2D(Box2DBody2D *p_body) :
			body(p_body) {}

	// virtual Vector2 _get_total_gravity() const override;
	// virtual double _get_total_angular_damp() const override;
	// virtual double _get_total_linear_damp() const override;

	Vector2 _get_center_of_mass() const override;
	Vector2 _get_center_of_mass_local() const override;
	float _get_inverse_mass() const override;
	float _get_inverse_inertia() const override;

	void _set_linear_velocity(const Vector2 &p_velocity) override;
	Vector2 _get_linear_velocity() const override;

	void _set_angular_velocity(float p_velocity) override;
	float _get_angular_velocity() const override;

	void _set_transform(const Transform2D &p_transform) override;
	Transform2D _get_transform() const override;

	Vector2 _get_velocity_at_local_position(const Vector2 &p_position) const override;

	void _apply_central_impulse(const Vector2 &p_impulse) override;
	void _apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position = Vector2()) override;
	void _apply_torque_impulse(float p_torque) override;

	void _apply_central_force(const Vector2 &p_force) override;
	void _apply_force(const Vector2 &p_force, const Vector2 &p_position = Vector2()) override;
	void _apply_torque(float p_torque) override;

	void _add_constant_central_force(const Vector2 &p_force) override;
	void _add_constant_force(const Vector2 &p_force, const Vector2 &p_position = Vector2()) override;
	void _add_constant_torque(float p_torque) override;
	void _set_constant_force(const Vector2 &p_force) override;
	Vector2 _get_constant_force() const override;
	void _set_constant_torque(float p_torque) override;
	float _get_constant_torque() const override;

	void _set_sleep_state(bool p_enable) override;
	bool _is_sleeping() const override;

	int _get_contact_count() const override;
	Vector2 _get_contact_local_position(int p_contact_idx) const override;
	Vector2 _get_contact_local_normal(int p_contact_idx) const override;
	int _get_contact_local_shape(int p_contact_idx) const override;
	RID _get_contact_collider(int p_contact_idx) const override;
	Vector2 _get_contact_collider_position(int p_contact_idx) const override;
	uint64_t _get_contact_collider_id(int p_contact_idx) const override;
	int _get_contact_collider_shape(int p_contact_idx) const override;
	Vector2 _get_contact_impulse(int p_contact_idx) const override;
	Vector2 _get_contact_collider_velocity_at_position(int p_contact_idx) const override;

	PhysicsDirectSpaceState2D *_get_space_state() override;

	float _get_step() const override;
};