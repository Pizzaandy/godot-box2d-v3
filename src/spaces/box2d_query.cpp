
#include "box2d_query.h"
#include "../bodies/box2d_collision_object_2d.h"

bool BodyQueryFilter::is_excluded(const Box2DCollisionObject2D *p_object) const {
	return B2_ID_EQUALS(p_object->get_body_id(), body_id);
}

bool SpaceStateQueryFilter::is_excluded(const Box2DCollisionObject2D *p_object) const {
	return space_state->is_body_excluded_from_query(p_object->get_rid());
}

bool ArrayQueryFilter::is_excluded(const Box2DCollisionObject2D *p_object) const {
	return exclude.has(p_object->get_rid());
}

int box2d_cast_shape(const Box2DShapePrimitive &p_shape, const CastQuery p_query, LocalVector<CastHit> &p_results) {
	CastQueryCollector collector(p_query, p_results);
	b2ShapeProxy proxy = p_shape.inflated(p_query.margin).get_proxy();
	b2World_CastShape(p_query.world, &proxy, to_box2d(p_query.translation), p_query.filter.box2d_filter, cast_callback, &collector);
	return collector.count;
}

int box2d_overlap_shape(const Box2DShapePrimitive &p_shape, const OverlapQuery p_query, LocalVector<ShapeOverlap> &p_results) {
	Box2DShapePrimitive shape = p_shape.inflated(p_query.margin);
	b2ShapeProxy proxy = shape.get_proxy();
	OverlapQueryCollector collector(p_query, p_results, shape);
	b2World_OverlapShape(p_query.world, &proxy, p_query.filter.box2d_filter, overlap_callback, &collector);
	return collector.count;
}

/// context = OverlapQueryCollector
bool overlap_callback(b2ShapeId shapeId, void *context) {
	OverlapQueryCollector *collector = static_cast<OverlapQueryCollector *>(context);
	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (!collector->filter.is_excluded(object)) {
		collector->results.push_back(ShapeOverlap{ object, shape, shapeId, collector->shape });
		collector->count++;
	}

	return collector->results.size() < collector->max_results;
}

/// context = CastQueryCollector
float cast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	CastQueryCollector *collector = static_cast<CastQueryCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(object)) {
		return -1;
	}

	if (fraction <= 0.0f && collector->ignore_initial_overlaps) {
		return -1;
	}

	collector->results.push_back(CastHit{ object, shape, shapeId, to_godot(point), to_godot(normal), fraction });
	collector->count++;

	if (collector->find_nearest) {
		return fraction;
	} else if (collector->results.size() >= collector->max_results) {
		return 0;
	}

	return 1;
}

int find_nearest_cast_hit(LocalVector<CastHit> &p_results) {
	ERR_FAIL_COND_V(p_results.size() == 0, -1);
	int nearest_index = 0;
	for (int i = 0; i < p_results.size(); i++) {
		if (p_results[i].fraction < p_results[nearest_index].fraction) {
			nearest_index = i;
		}
	}
	return nearest_index;
}
