#include "../bodies/box2d_body_2d.h"
#include "box2d_physics_direct_space_state_2d.h"

struct QueryFilter {
	Box2DDirectSpaceState2D *space_state = nullptr;
	TypedArray<RID> *exclude = nullptr;

	bool is_excluded(const RID &p_body) {
		if (space_state) {
			return space_state->is_body_excluded_from_query(p_body);
		} else {
			return exclude->has(p_body);
		}
	}

	QueryFilter(Box2DDirectSpaceState2D *p_space_state) :
			space_state(p_space_state) {}

	QueryFilter(TypedArray<RID> *p_exclude) :
			exclude(p_exclude) {}
};

struct ShapeOverlap {
	Box2DBody2D *body = nullptr;
	Box2DShapeInstance *shape = nullptr;
	b2ShapeId shape_id;
};

struct ShapeOverlapCollector {
	int max_results = 0;
	Vector<ShapeOverlap> overlaps;
	QueryFilter filter;
	ShapeOverlapCollector(int p_max_results, QueryFilter &p_context) :
			filter(p_context),
			max_results(p_max_results) {}
};

bool overlap_callback(b2ShapeId shapeId, void *context) {
	ShapeOverlapCollector *collector = static_cast<ShapeOverlapCollector *>(context);
	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid())) {
		return true;
	}

	collector->overlaps.push_back(ShapeOverlap{ body, shape, shapeId });

	return collector->overlaps.size() < collector->max_results;
}

struct CastHit {
	Box2DBody2D *body;
	Box2DShapeInstance *shape;
	b2ShapeId shape_id;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction;
};

struct NearestCastHitCollector {
	CastHit nearest_hit;
	QueryFilter filter;
	bool hit = false;
	NearestCastHitCollector(QueryFilter &p_filter) :
			filter(p_filter) {}
};

float cast_callback_nearest(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	NearestCastHitCollector *collector = static_cast<NearestCastHitCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid())) {
		return -1;
	}

	collector->hit = true;
	collector->nearest_hit = CastHit{ body, shape, shapeId, point, normal, fraction };

	return fraction;
}

struct CastHitCollector {
	int max_results = 0;
	QueryFilter filter;
	bool hit = false;
	Vector<CastHit> hits;

	CastHitCollector(int p_max_results, QueryFilter &p_filter) :
			max_results(p_max_results), filter(p_filter) {}
};

float cast_callback_all(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	CastHitCollector *collector = static_cast<CastHitCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->filter.is_excluded(body->get_rid())) {
		return -1;
	}

	collector->hit = true;
	collector->hits.push_back(CastHit{ body, shape, shapeId, point, normal, fraction });

	if (collector->hits.size() >= collector->max_results) {
		return 0;
	}

	return 1;
}