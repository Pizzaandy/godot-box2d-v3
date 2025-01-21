#pragma once

#include "box2d_collision_object_2d.h"
#include <godot_cpp/classes/physics_server2d_extension.hpp>

using namespace godot;

class Box2DArea2D final : public Box2DCollisionObject2D {
public:
	Box2DArea2D();

	void step();

	void set_gravity_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) { override_gravity_mode = p_mode; }
	PhysicsServer2D::AreaSpaceOverrideMode get_gravity_override_mode() const { return override_gravity_mode; }

	void set_linear_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) { override_linear_damp_mode = p_mode; }
	PhysicsServer2D::AreaSpaceOverrideMode get_linear_damp_override_mode() const { return override_linear_damp_mode; }

	void set_angular_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) { override_angular_damp_mode = p_mode; }
	PhysicsServer2D::AreaSpaceOverrideMode get_angular_damp_override_mode() const { return override_angular_damp_mode; }

	void set_linear_damp(float p_damp) { linear_damp = p_damp; }
	float get_linear_damp() const { return linear_damp; }

	void set_angular_damp(float p_damp) { angular_damp = p_damp; }
	float get_angular_damp() const { return angular_damp; }

	void set_gravity_strength(float p_strength) { gravity_strength = p_strength; }
	float get_gravity_strength() const { return gravity_strength; }

	void set_gravity_direction(Vector2 p_direction) { gravity_direction = p_direction; }
	Vector2 get_gravity_direction() const { return gravity_direction; }

	void set_gravity_point_enabled(bool p_enabled) { gravity_point_enabled = p_enabled; }

	void set_gravity_point_unit_distance(float p_distance) { gravity_point_unit_distance = p_distance; }
	float get_gravity_point_unit_distance() const { return gravity_point_unit_distance; }

	void set_priority(float p_priority) { priority = p_priority; }
	int get_priority() const { return priority; }

	void set_monitorable(float p_monitorable) { monitorable = p_monitorable; }
	bool get_monitorable() const { return monitorable; }

	void set_monitor_callback(const Callable &p_callback) {}

	bool operator<(const Box2DArea2D &other) const {
		return get_priority() < other.get_priority();
	}

private:
	int priority = 0;
	bool monitorable = false;

	float gravity_strength = 9.80665;
	Vector2 gravity_direction = Vector2(0, -1);
	float linear_damp = 0.1;
	float angular_damp = 1.0;

	bool gravity_point_enabled = false;
	float gravity_point_unit_distance = 0.0;

	PhysicsServer2D::AreaSpaceOverrideMode override_gravity_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_linear_damp_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_angular_damp_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
};