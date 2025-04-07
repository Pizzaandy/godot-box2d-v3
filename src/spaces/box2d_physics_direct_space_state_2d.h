#pragma once

#include "../box2d_globals.h"

#include <godot_cpp/classes/physics_direct_space_state2d_extension.hpp>
#include <godot_cpp/templates/local_vector.hpp>

using namespace godot;

class Box2DSpace2D;
class CastHit;
class ShapeOverlap;

class Box2DDirectSpaceState2D : public PhysicsDirectSpaceState2DExtension {
	GDCLASS(Box2DDirectSpaceState2D, PhysicsDirectSpaceState2DExtension);

public:
	Box2DDirectSpaceState2D() = default;
	Box2DDirectSpaceState2D(Box2DSpace2D *p_space) :
			space(p_space) {};

	bool _intersect_ray(const Vector2 &p_from, const Vector2 &p_to, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, bool p_hit_from_inside, PhysicsServer2DExtensionRayResult *p_result) override;
	int32_t _intersect_point(const Vector2 &p_position, uint64_t p_canvas_instance_id, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeResult *p_results, int32_t p_max_results) override;
	int32_t _intersect_shape(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, float p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeResult *p_result, int32_t p_max_results) override;
	bool _cast_motion(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, float p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, float *p_closest_safe, float *p_closest_unsafe) override;
	bool _collide_shape(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, float p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, void *p_results, int32_t p_max_results, int32_t *p_result_count) override;
	bool _rest_info(const RID &p_shape_rid, const Transform2D &p_transform, const Vector2 &p_motion, float p_margin, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, PhysicsServer2DExtensionShapeRestInfo *p_rest_info) override;

	Dictionary cast_shape(const Ref<PhysicsShapeQueryParameters2D> &p_parameters);
	TypedArray<Dictionary> cast_shape_all(const Ref<PhysicsShapeQueryParameters2D> &p_parameters, int32_t p_max_results = 32);

	static Object *get_instance_hack(uint64_t p_object_id) {
		// ????
		return reinterpret_cast<Object *>((GodotObject *)(internal::gdextension_interface_object_get_instance_from_id(p_object_id)));
	}

	b2QueryFilter make_filter(uint64_t p_collision_mask, bool p_collide_bodies, bool p_collide_areas);

private:
	static void _bind_methods();

	Box2DSpace2D *space = nullptr;
};