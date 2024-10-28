#pragma once

#include "box2d_collision_object_2d.h"
#include <godot_cpp/classes/physics_server2d_extension.hpp>

using namespace godot;

class Box2DArea2D : public Box2DCollisionObject2D {
public:
	void step();

	void set_gravity_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
		override_gravity = p_mode;
	}

	void set_linear_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
		override_linear_damp = p_mode;
	}

	void set_angular_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
		override_angular_damp = p_mode;
	}

	void set_gravity_strength(float p_strength) { gravity_strength = p_strength; }
	float get_gravity_strength() const { return gravity_strength; }

	void set_gravity_direction(Vector2 p_direction) { gravity_direction = p_direction; }
	Vector2 get_gravity_direction() const { return gravity_direction; }

	void set_gravity_point_enabled(bool p_enabled) { gravity_point_enabled = p_enabled; }

	void set_gravity_point_unit_distance(float p_distance) { gravity_point_unit_distance = p_distance; }
	float get_gravity_point_unit_distance() const { return gravity_point_unit_distance; }

	int get_priority() const { return priority; }
	void set_priority(float p_priority) { priority = p_priority; }

private:
	int priority = 0;

	float gravity_strength = 9.80665;
	Vector2 gravity_direction = Vector2(0, -1);
	float linear_damp = 0.1;
	float angular_damp = 1.0;

	bool gravity_point_enabled = false;
	float gravity_point_unit_distance = 0.0;

	PhysicsServer2D::AreaSpaceOverrideMode override_gravity = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_linear_damp = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_angular_damp = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
};