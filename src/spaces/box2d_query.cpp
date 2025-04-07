
#include "box2d_query.h"
#include "../bodies/box2d_collision_object_2d.h"

bool SpaceStateQueryFilter::is_excluded(const Box2DCollisionObject2D *p_object) const {
	return space_state->is_body_excluded_from_query(p_object->get_rid());
}

bool ArrayQueryFilter::is_excluded(const Box2DCollisionObject2D *p_object) const {
	return exclude.has(p_object->get_rid());
}

/// context = ShapeOverlapQueryCollector
bool overlap_callback(b2ShapeId shapeId, void *context) {
	OverlapQueryCollector *collector = static_cast<OverlapQueryCollector *>(context);
	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (!collector->filter.is_excluded(object)) {
		collector->results.push_back(ShapeOverlap{ object, shape, shapeId });
	}

	return collector->results.size() < collector->max_results;
}

/// context = ShapeCastQueryCollector
float cast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	CastQueryCollector *collector = static_cast<CastQueryCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *object = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(object)) {
		return -1;
	}

	collector->results.ordered_insert(CastHit{ object, shape, shapeId, point, normal, fraction });

	if (collector->find_nearest) {
		return fraction;
	} else if (collector->results.size() >= collector->max_results) {
		return 0;
	}

	return 1;
}