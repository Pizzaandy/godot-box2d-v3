
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../servers/box2d_physics_server_2d.h"
#include "box2d_query.h"

#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>

static thread_local LocalVector<CastHit> cast_results;
static thread_local LocalVector<ShapeOverlap> overlap_results;

void Box2DDirectSpaceState2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("cast_shape"), &Box2DDirectSpaceState2D::cast_shape, "parameters");
	ClassDB::bind_method(D_METHOD("cast_shape_all"), &Box2DDirectSpaceState2D::cast_shape_all, "parameters", "max_results");
}

b2QueryFilter Box2DDirectSpaceState2D::make_filter(uint64_t p_collision_mask, bool p_collide_bodies, bool p_collide_areas) {
	b2QueryFilter result = b2QueryFilter{ 0, p_collision_mask };
	if (p_collide_bodies) {
		result.categoryBits |= BODY_MASK_BIT;
	}
	if (p_collide_areas) {
		result.categoryBits |= AREA_MASK_BIT;
	}
	return result;
}

int32_t Box2DDirectSpaceState2D::_intersect_point(
		const Vector2 &p_position,
		uint64_t p_canvas_instance_id,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeResult *p_results,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, 0);

	if (p_max_results <= 0) {
		return 0;
	}

	b2Transform transform = b2Transform_identity;
	transform.p = to_box2d(p_position);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	overlap_results.clear();
	OverlapQueryCollector collector(p_max_results, query_filter, overlap_results);
	b2ShapeProxy proxy = b2MakeProxy(&transform.p, 1, 0.0);
	b2World_OverlapShape(space->get_world_id(), &proxy, box2d_filter, overlap_callback, &collector);

	for (ShapeOverlap overlap : collector.results) {
		ERR_FAIL_COND_V(overlap.shape->get_index() < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_results++;

		result.shape = overlap.shape->get_index();
		result.rid = overlap.object->get_rid();
		result.collider_id = overlap.object->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return collector.count;
}

bool Box2DDirectSpaceState2D::_intersect_ray(
		const Vector2 &p_from,
		const Vector2 &p_to,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		bool p_hit_from_inside,
		PhysicsServer2DExtensionRayResult *p_result) {
	ERR_FAIL_COND_V(!space, 0);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	if (p_hit_from_inside) {
		overlap_results.clear();
		OverlapQueryCollector collector(1, query_filter, overlap_results);
		b2Vec2 from = to_box2d(p_from);
		b2ShapeProxy proxy = b2MakeProxy(&from, 1, 0.0);
		b2World_OverlapShape(space->get_world_id(), &proxy, box2d_filter, overlap_callback, &collector);

		if (collector.count > 0) {
			ShapeOverlap overlap = collector.results[0];
			ERR_FAIL_COND_V(overlap.shape->get_index() < 0, false);

			p_result->position = p_from;
			p_result->normal = Vector2();
			p_result->shape = overlap.shape->get_index();
			p_result->rid = overlap.object->get_rid();
			p_result->collider_id = overlap.object->get_instance_id();
			if (p_result->collider_id.is_valid()) {
				p_result->collider = get_instance_hack(p_result->collider_id);
			}
			return true;
		}
	}

	cast_results.clear();
	CastQueryCollector collector(1, query_filter, true, cast_results);
	b2World_CastRay(space->get_world_id(), to_box2d(p_from), to_box2d(p_to - p_from), box2d_filter, cast_callback, &collector);

	if (collector.count == 0) {
		return false;
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, 0);

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

	PhysicsServer2DExtensionRayResult &result = *p_result;

	result.position = to_godot(hit.point);
	result.normal = to_godot_normalized(hit.normal);
	result.shape = hit.shape->get_index();
	result.rid = hit.object->get_rid();
	result.collider_id = hit.object->get_instance_id();
	if (result.collider_id.is_valid()) {
		result.collider = get_instance_hack(result.collider_id);
	}

	return true;
}

int32_t Box2DDirectSpaceState2D::_intersect_shape(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeResult *p_result,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, 0);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, 0);

	if (p_max_results <= 0) {
		return 0;
	}

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.transform = p_transform;
	query.translation = p_motion;
	query.find_nearest = false;

	cast_results.clear();
	shape->cast(query, cast_results);

	int count = 0;
	for (CastHit &hit : cast_results) {
		ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_result++;
		result.shape = hit.shape->get_index();
		result.rid = hit.object->get_rid();
		result.collider_id = hit.object->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}

		count++;
	}

	return count;
}

bool Box2DDirectSpaceState2D::_cast_motion(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		float *p_closest_safe,
		float *p_closest_unsafe) {
	ERR_FAIL_COND_V(!space, false);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_NULL_V(shape, false);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.transform = p_transform;
	query.translation = p_motion;
	query.find_nearest = true;

	cast_results.clear();
	int count = shape->cast(query, cast_results);

	if (count == 0) {
		*p_closest_safe = 1.0;
		*p_closest_unsafe = 1.0;
		return true;
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, 0);

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

	*p_closest_safe = box2d_compute_safe_fraction(hit.fraction, p_motion.length());
	*p_closest_unsafe = hit.fraction;

	return true;
}

bool Box2DDirectSpaceState2D::_collide_shape(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		void *p_results,
		int32_t p_max_results,
		int32_t *p_result_count) {
	ERR_FAIL_COND_V(!space, false);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, false);

	if (p_max_results <= 0) {
		return 0;
	}

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	OverlapQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.transform = p_transform;

	overlap_results.clear();
	int count = shape->overlap(query, overlap_results);

	if (count == 0) {
		return false;
	}

	Vector2 *points = static_cast<Vector2 *>(p_results);
	int max_points = p_max_results * 2;
	int point_count = 0;

	return false;
}

bool Box2DDirectSpaceState2D::_rest_info(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeRestInfo *p_rest_info) {
	ERR_FAIL_COND_V(!space, false);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, false);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	OverlapQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.transform = p_transform;

	overlap_results.clear();
	int count = shape->overlap(query, overlap_results);

	if (count == 0) {
		return false;
	}

	// TODO: implement

	return false;
}

Dictionary Box2DDirectSpaceState2D::cast_shape(const Ref<PhysicsShapeQueryParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(!space, {});
	PhysicsShapeQueryParameters2D *params = p_parameters.ptr();
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(params->get_shape_rid());
	ERR_FAIL_COND_V(!shape, {});

	b2QueryFilter box2d_filter = make_filter(
			params->get_collision_mask(),
			params->is_collide_with_bodies_enabled(),
			params->is_collide_with_areas_enabled());

	ArrayQueryFilter query_filter(params->get_exclude(), box2d_filter);

	Transform2D transform = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.transform = transform;
	query.translation = motion;
	query.find_nearest = true;

	cast_results.clear();
	int count = shape->cast(query, cast_results);

	if (count == 0) {
		return {};
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, {});

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, {});

	Dictionary result;
	result["point"] = to_godot(hit.point);
	result["destination"] = transform.get_origin() + (hit.fraction * motion);
	result["normal"] = to_godot_normalized(hit.normal);
	result["shape"] = hit.shape->get_index();
	result["rid"] = hit.object->get_rid();
	ObjectID id = hit.object->get_instance_id();
	result["collider_id"] = id;
	if (id.is_valid()) {
		result["collider"] = get_instance_hack(id);
	}

	return result;
}

TypedArray<Dictionary> Box2DDirectSpaceState2D::cast_shape_all(
		const Ref<PhysicsShapeQueryParameters2D> &p_parameters,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, {});
	PhysicsShapeQueryParameters2D *params = p_parameters.ptr();
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(params->get_shape_rid());
	ERR_FAIL_COND_V(!shape, {});

	if (p_max_results <= 0) {
		return {};
	}

	b2QueryFilter box2d_filter = make_filter(
			params->get_collision_mask(),
			params->is_collide_with_bodies_enabled(),
			params->is_collide_with_areas_enabled());

	ArrayQueryFilter query_filter(params->get_exclude(), box2d_filter);

	Transform2D transform = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.transform = transform;
	query.translation = motion;
	query.find_nearest = false;

	cast_results.clear();
	int count = shape->cast(query, cast_results);

	if (count == 0) {
		return {};
	}

	TypedArray<Dictionary> hits;

	for (CastHit &hit : cast_results) {
		Dictionary result;
		result["point"] = to_godot(hit.point);
		result["destination"] = transform.get_origin() + (hit.fraction * motion);
		result["normal"] = to_godot_normalized(hit.normal);
		result["shape"] = hit.shape->get_index();
		result["rid"] = hit.object->get_rid();
		ObjectID id = hit.object->get_instance_id();
		result["collider_id"] = id;
		if (id.is_valid()) {
			result["collider"] = get_instance_hack(id);
		}
		hits.push_back(result);
	}

	return hits;
}