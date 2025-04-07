#include "box2d_area_2d.h"
#include "../spaces/box2d_space_2d.h"
#include "box2d_body_2d.h"
#include <godot_cpp/templates/local_vector.hpp>

Box2DArea2D::Box2DArea2D() :
		Box2DCollisionObject2D(Type::AREA) {
	body_def.type = b2_kinematicBody;
	mode = PhysicsServer2D::BODY_MODE_KINEMATIC;
	shape_def.isSensor = true;

	const uint32_t default_bitmask = 1u << 0;
	set_collision_layer(default_bitmask);
	set_collision_mask(default_bitmask);
}

void Box2DArea2D::body_created() {
	update_area_step_list();
}

void Box2DArea2D::body_destroyed() {
	if (in_area_step_list) {
		space->remove_active_area(this);
		in_area_step_list = false;
	}

	// Areas do not emit events when they are freed - consistent with Godot Physics

	// const auto overlaps_copy = overlaps;
	// for (const auto &[shape_pair, data] : overlaps_copy) {
	// 	for (int i = 0; i < data.count; i++) {
	// 		remove_overlap(shape_pair.other_shape, shape_pair.self_shape);
	// 	}
	// }
}

void Box2DArea2D::shapes_changed() {
	update_overlaps();
}

void Box2DArea2D::add_overlap(Box2DShapeInstance *p_other_shape, Box2DShapeInstance *p_self_shape) {
	ShapePair pair{ p_other_shape, p_self_shape };

	if (++overlaps[pair].count == 1) {
		Box2DCollisionObject2D *object = p_other_shape->get_collision_object();
		report_event(
				object->get_type(),
				PhysicsServer2D::AREA_BODY_ADDED,
				object,
				p_other_shape->get_index(),
				p_self_shape->get_index());

		object_overlap_count[p_other_shape->get_collision_object()]++;
	}

	overlaps[pair].object = p_other_shape->get_collision_object();
}

void Box2DArea2D::remove_overlap(Box2DShapeInstance *p_other_shape, Box2DShapeInstance *p_self_shape) {
	ShapePair pair{ p_other_shape, p_self_shape };

	if (--overlaps[pair].count <= 0) {
		Box2DCollisionObject2D *object = overlaps[pair].object;
		overlaps.erase(pair);

		report_event(
				object->get_type(),
				PhysicsServer2D::AREA_BODY_REMOVED,
				object,
				p_other_shape->get_index(),
				p_self_shape->get_index());

		if (--object_overlap_count[object] <= 0) {
			object_overlap_count.erase(object);
			Box2DBody2D *body = object->as_body();
			if (body) {
				body->apply_area_overrides();
			}
		}
	}
}

void Box2DArea2D::update_overlaps() {
	// TODO: optimize

	HashMap<ShapePair, int, ShapePair> new_overlaps;

	for (const auto &[shape_pair, data] : overlaps) {
		new_overlaps[shape_pair] = 0;
	}

	if (in_space) {
		LocalVector<b2ShapeId> shape_overlaps;
		shape_overlaps.reserve(overlaps.size());

		BodyShapeRange range(body_id);
		for (b2ShapeId shape_id : range) {
			Box2DShapeInstance *self_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shape_id));

			int capacity = b2Shape_GetSensorCapacity(shape_id);

			if (capacity > shape_overlaps.size()) {
				shape_overlaps.resize(capacity);
			}

			int overlap_count = b2Shape_GetSensorOverlaps(shape_id, shape_overlaps.ptr(), capacity);

			for (int i = 0; i < overlap_count; i++) {
				Box2DShapeInstance *other_shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shape_overlaps[i]));
				ShapePair pair{ other_shape, self_shape };
				new_overlaps[pair]++;
			}
		}
	}

	// TODO: Evaluate whether to make this consistent with Godot Physics.
	// Godot Physics exits all its current overlaps shapes and re-enters the ones that are remaining.
	// This code, which only reports differences in overlaps, is more intuitive and consistent with how Areas are presented in the docs.

	for (const auto &[shape_pair, count] : new_overlaps) {
		auto it = overlaps.find(shape_pair);

		if (it == overlaps.end()) {
			for (int i = 0; i < count; i++) {
				add_overlap(shape_pair.other_shape, shape_pair.self_shape);
			}
			continue;
		}

		int difference = count - it->value.count;

		if (difference == 0) {
			continue;
		} else if (difference > 0) {
			for (int i = 0; i < difference; i++) {
				add_overlap(shape_pair.other_shape, shape_pair.self_shape);
			}
		} else {
			for (int i = 0; i < Math::abs(difference); i++) {
				remove_overlap(shape_pair.other_shape, shape_pair.self_shape);
			}
		}
	}
}

void Box2DArea2D::apply_overrides() {
	for (const auto &[object, overlap_count] : object_overlap_count) {
		Box2DBody2D *body = object->as_body();
		if (!body || object->is_freed()) {
			continue;
		}

		Box2DBody2D::AreaOverrideAccumulator &overrides = body->area_overrides;

		if (!overrides.ignore_remaining_gravity) {
			Vector2 original_gravity = overrides.total_gravity;
			overrides.ignore_remaining_gravity = integrate(
					overrides.total_gravity,
					compute_gravity(body->get_center_of_mass_global()),
					override_gravity_mode);

			if (override_gravity_mode == PhysicsServer2D::AREA_SPACE_OVERRIDE_REPLACE ||
					override_gravity_mode == PhysicsServer2D::AREA_SPACE_OVERRIDE_REPLACE_COMBINE) {
				overrides.skip_world_gravity = true;
			}
		}

		if (!overrides.ignore_remaining_linear_damp) {
			float original_linear_damp = overrides.total_linear_damp;
			overrides.ignore_remaining_linear_damp = integrate(
					overrides.total_linear_damp,
					linear_damp,
					override_linear_damp_mode);
		}

		if (!overrides.ignore_remaining_angular_damp) {
			float original_angular_damp = overrides.total_angular_damp;
			overrides.ignore_remaining_angular_damp = integrate(
					overrides.total_angular_damp,
					angular_damp,
					override_angular_damp_mode);
		}

		ERR_FAIL_NULL(space);
		space->add_body_with_overrides(body);
	}
}

void Box2DArea2D::update_area_step_list() {
	if (!in_space || is_default_area()) {
		return;
	}

	ERR_FAIL_NULL(space);

	bool has_gravity = override_gravity_mode != PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	bool has_linear_damp = override_linear_damp_mode != PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;
	bool has_angular_damp = override_angular_damp_mode != PhysicsServer2D::AREA_SPACE_OVERRIDE_DISABLED;

	bool has_overrides = has_gravity || has_linear_damp || has_angular_damp;

	if (has_overrides) {
		if (!in_area_step_list) {
			space->add_active_area(this);
			in_area_step_list = true;
		}
	} else {
		if (in_area_step_list) {
			space->remove_active_area(this);
			in_area_step_list = false;
		}
	}
}

void Box2DArea2D::report_event(
		Type p_type,
		PhysicsServer2D::AreaBodyStatus p_status,
		Box2DCollisionObject2D *p_other_object,
		int32_t p_other_shape_index,
		int32_t p_self_shape_index) {
	static thread_local Array arguments = []() {
		Array array;
		array.resize(5);
		return array;
	}();

	arguments[0] = p_status;
	arguments[1] = p_other_object->get_rid();
	arguments[2] = p_other_object->get_instance_id();
	arguments[3] = p_other_shape_index;
	arguments[4] = p_self_shape_index;

	if (p_type == Type::AREA) {
		ERR_FAIL_COND(!area_monitor_callback.is_valid());
		area_monitor_callback.callv(arguments);
	} else {
		ERR_FAIL_COND(!body_monitor_callback.is_valid());
		body_monitor_callback.callv(arguments);
	}
}

bool Box2DArea2D::is_default_area() const {
	return space && space->get_default_area() == this;
}

void Box2DArea2D::set_gravity_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
	override_gravity_mode = p_mode;
	update_area_step_list();
}

void Box2DArea2D::set_linear_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
	override_linear_damp_mode = p_mode;
	update_area_step_list();
}

void Box2DArea2D::set_angular_damp_override_mode(PhysicsServer2D::AreaSpaceOverrideMode p_mode) {
	override_angular_damp_mode = p_mode;
	update_area_step_list();
}

void Box2DArea2D::set_linear_damp(float p_damp) {
	linear_damp = p_damp;
	update_area_step_list();
}

void Box2DArea2D::set_angular_damp(float p_damp) {
	angular_damp = p_damp;
	update_area_step_list();
}

void Box2DArea2D::set_gravity_strength(float p_strength) {
	gravity_strength = p_strength;
	update_area_step_list();
	gravity_changed();
}

void Box2DArea2D::set_gravity_direction(Vector2 p_direction) {
	gravity_direction = p_direction.normalized();
	update_area_step_list();
	gravity_changed();
}

void Box2DArea2D::set_gravity_point_enabled(bool p_enabled) {
	gravity_point_enabled = p_enabled;
	update_area_step_list();
}

Vector2 Box2DArea2D::compute_gravity(Vector2 p_position) const {
	if (gravity_point_enabled) {
		Vector2 displacement = get_transform().xform(gravity_direction) - p_position;

		if (gravity_point_unit_distance <= 0.0) {
			return displacement.normalized() * gravity_strength;
		}

		float distance_sq = displacement.length_squared();

		if (distance_sq <= 0.0) {
			return Vector2();
		}

		float adjusted_strength = gravity_strength * (gravity_point_unit_distance * gravity_point_unit_distance) / distance_sq;

		return displacement.normalized() * adjusted_strength;
	}

	return gravity_direction * gravity_strength;
}

void Box2DArea2D::gravity_changed() {
	if (is_default_area()) {
		space->set_default_gravity(gravity_direction * gravity_strength);
	}
}

void Box2DArea2D::set_priority(float p_priority) {
	priority = p_priority;

	if (in_area_step_list) {
		ERR_FAIL_NULL(space);
		space->remove_active_area(this);
		space->add_active_area(this);
	}
}

void Box2DArea2D::set_monitorable(float p_monitorable) {
	monitorable = p_monitorable;

	set_collision_mask(shape_def.filter.maskBits);
}

uint64_t Box2DArea2D::modify_mask_bits(uint32_t p_mask) {
	uint64_t result = (uint64_t)p_mask | AREA_MASK_BIT;

	if (monitorable) {
		result |= AREA_MONITORABLE_MASK_BIT;
	}

	return result;
}