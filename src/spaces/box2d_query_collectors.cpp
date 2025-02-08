
#include "box2d_query_collectors.h"
#include "../bodies/box2d_collision_object_2d.h"

/// context = ShapeOverlapCollector
bool overlap_callback(b2ShapeId shapeId, void *context) {
	ShapeOverlapCollector *collector = static_cast<ShapeOverlapCollector *>(context);
	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *body = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid(), body->is_area())) {
		return true;
	}

	if (collector->contains_id(shapeId)) {
		return true;
	}

	collector->overlaps.push_back(ShapeOverlap{ body, shape, shapeId });

	return collector->overlaps.size() < collector->max_results;
}

/// context = NearestCastHitCollector
float cast_callback_nearest(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	NearestCastHitCollector *collector = static_cast<NearestCastHitCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *body = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid(), body->is_area())) {
		return -1;
	}

	collector->hit = true;
	collector->nearest_hit = CastHit{ body, shape, shapeId, point, normal, fraction };

	return fraction;
}

/// context = CastHitCollector
float cast_callback_all(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	CastHitCollector *collector = static_cast<CastHitCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DCollisionObject2D *body = static_cast<Box2DCollisionObject2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid(), body->is_area())) {
		return -1;
	}

	if (collector->contains_id(shapeId)) {
		return -1;
	}

	collector->hits.push_back(CastHit{ body, shape, shapeId, point, normal, fraction });

	if (collector->hits.size() >= collector->max_results) {
		return 0;
	}

	return 1;
}