#pragma once

#include "box2d_space_2d.h"
#include <godot_cpp/classes/physics_direct_space_state2d_extension.hpp>

using namespace godot;

class Box2DSpace2D;

class Box2DPhysicsDirectSpaceState2D : public PhysicsDirectSpaceState2DExtension {
	GDCLASS(Box2DPhysicsDirectSpaceState2D, PhysicsDirectSpaceState2DExtension);

public:
	Box2DPhysicsDirectSpaceState2D() = default;
	Box2DPhysicsDirectSpaceState2D(Box2DSpace2D *p_space) :
			space(p_space) {};

	bool _intersect_ray(const Vector2 &p_from, const Vector2 &p_to, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, bool p_hit_from_inside, PhysicsServer2DExtensionRayResult *p_result) override;
	int32_t _intersect_point(const Vector2 &p_position, uint64_t p_canvas_instance_id, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeResult *p_results, int32_t p_max_results) override;
	// int32_t _intersect_shape(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, double p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeResult *p_result, int32_t p_max_results) override;
	// bool _cast_motion(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, double p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, float *p_closest_safe, float *p_closest_unsafe) override;
	// bool _collide_shape(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, double p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, void *p_results, int32_t p_max_results, int32_t *p_result_count) override;
	// bool _rest_info(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, double p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeRestInfo *p_rest_info) override;

	static Object *get_instance_hack(uint64_t p_object_id) {
		// ????
		return reinterpret_cast<Object *>((GodotObject *)(internal::gdextension_interface_object_get_instance_from_id(p_object_id)));
	}

private:
	static void _bind_methods();

	Box2DSpace2D *space = nullptr;
};