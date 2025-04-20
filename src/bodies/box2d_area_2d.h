#pragma once

#include "box2d_collision_object_2d.h"
#include <godot_cpp/classes/physics_server2d_extension.hpp>

using namespace godot;

class Box2DBody2D;

template <typename T>
bool integrate(T &p_value, const T &p_override_value, PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
	switch (p_mode) {
		case PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED: {
			return false;
		}
		case PhysicsServer2D::AREA_SPACE_OVERRIDE_COMBINE: {
			p_value += p_override_value;
			return false;
		}
		case PhysicsServer2D::AREA_SPACE_OVERRIDE_COMBINE_REPLACE: {
			p_value += p_override_value;
			return true;
		}
		case PhysicsServer2D::AREA_SPACE_OVERRIDE_REPLACE: {
			p_value = p_override_value;
			return true;
		}
		case PhysicsServer2D::AREA_SPACE_OVERRIDE_REPLACE_COMBINE: {
			p_value = p_override_value;
			return false;
		}
		default:
			ERR_FAIL_V(false);
	}
}

class Box2DArea2D final : public Box2DCollisionObject2D {
public:
	struct ShapePair {
		Box2DShapeInstance *other_shape = nullptr;
		Box2DShapeInstance *self_shape = nullptr;

		static uint32_t hash(const ShapePair &p_pair) {
			uint32_t hash = hash_murmur3_one_64((uint64_t)p_pair.other_shape);
			hash = hash_murmur3_one_64((uint64_t)p_pair.self_shape, hash);
			return hash_fmix32(hash);
		}

		bool operator==(const ShapePair &other) const {
			return other_shape == other.other_shape && self_shape == other.self_shape;
		}
	};

	struct ObjectAndOverlapCount {
		Box2DCollisionObject2D *object = nullptr;
		int count = 0;
	};

	Box2DArea2D();
	~Box2DArea2D();

	void apply_overrides();

	void update_area_step_list();

	void add_overlap(Box2DShapeInstance *p_other_shape, Box2DShapeInstance *p_self_shape);

	void remove_overlap(Box2DShapeInstance *p_other_shape, Box2DShapeInstance *p_self_shape);

	void update_overlaps();

	void report_event(
			Type p_type,
			PhysicsServer2D::AreaBodyStatus p_status,
			Box2DCollisionObject2D *p_other_object,
			int32_t p_other_shape_index,
			int32_t p_self_shape_index);

	bool is_default_area() const;

	void set_gravity_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode);
	PhysicsServer2D::AreaSpaceOverrideMode get_gravity_override_mode() const { return override_gravity_mode; }

	void set_linear_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode);
	PhysicsServer2D::AreaSpaceOverrideMode get_linear_damp_override_mode() const { return override_linear_damp_mode; }

	void set_angular_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode);
	PhysicsServer2D::AreaSpaceOverrideMode get_angular_damp_override_mode() const { return override_angular_damp_mode; }

	void set_linear_damp(float p_damp);
	float get_linear_damp() const { return linear_damp; }

	void set_angular_damp(float p_damp);
	float get_angular_damp() const { return angular_damp; }

	void set_gravity_strength(float p_strength);
	float get_gravity_strength() const { return gravity_strength; }

	void set_gravity_direction(Vector2 p_direction);
	Vector2 get_gravity_direction() const { return gravity_direction; }

	void set_gravity_point_enabled(bool p_enabled);
	bool get_gravity_point_enabled() const { return gravity_point_enabled; }

	void set_gravity_point_unit_distance(float p_distance) { gravity_point_unit_distance = p_distance; }
	float get_gravity_point_unit_distance() const { return gravity_point_unit_distance; }

	Vector2 compute_gravity(Vector2 p_center_of_mass) const;

	void gravity_changed();

	void set_priority(float p_priority);
	int get_priority() const { return priority; }

	void set_monitorable(bool p_monitorable);
	bool get_monitorable() const { return monitorable; }

	void set_body_monitor_callback(const Callable &p_callback) { body_monitor_callback = p_callback; }
	void set_area_monitor_callback(const Callable &p_callback) { area_monitor_callback = p_callback; }

	bool operator<(const Box2DArea2D &other) const {
		return get_priority() < other.get_priority();
	}

	void shapes_changed() override;

protected:
	void on_added_to_space() override;
	void on_remove_from_space() override;

	// `overlaps` is a HashMap to account for redundant sensor events from compound shapes.
	// value = number of simultaneous overlap events between two shape instances
	HashMap<ShapePair, ObjectAndOverlapCount, ShapePair> overlaps;
	HashMap<Box2DCollisionObject2D *, int> object_overlap_count;

	uint64_t modify_mask_bits(uint32_t p_mask) override;

	int priority = 0;
	bool monitorable = false;
	Callable body_monitor_callback;
	Callable area_monitor_callback;

	float gravity_strength = 9.8;
	Vector2 gravity_direction = Vector2(0, -1);
	float linear_damp = 0.1;
	float angular_damp = 0.1;

	bool gravity_point_enabled = false;
	float gravity_point_unit_distance = 0.0;

	bool in_area_step_list = false;

	PhysicsServer2D::AreaSpaceOverrideMode override_gravity_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_linear_damp_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	PhysicsServer2D::AreaSpaceOverrideMode override_angular_damp_mode = PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
};