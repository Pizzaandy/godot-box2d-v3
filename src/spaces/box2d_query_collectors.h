#include "../bodies/box2d_body_2d.h"
#include "box2d_physics_direct_space_state_2d.h"

struct ShapeOverlap {
	Box2DBody2D *body = nullptr;
	Box2DShapeInstance *shape = nullptr;
};

struct ShapeOverlapCollector {
	int max_results = 0;
	Box2DPhysicsDirectSpaceState2D *context;
	Vector<ShapeOverlap> shapes;
	ShapeOverlapCollector(int p_max_results, Box2DPhysicsDirectSpaceState2D *p_context) :
			max_results(p_max_results), context(p_context) {}
};

bool overlap_callback(b2ShapeId shapeId, void *context) {
	ShapeOverlapCollector *collector = static_cast<ShapeOverlapCollector *>(context);
	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->context->is_body_excluded_from_query(body->get_rid())) {
		return true;
	}

	collector->shapes.push_back(ShapeOverlap{ body, shape });

	return collector->shapes.size() < collector->max_results;
}

struct CastHit {
	Box2DBody2D *body;
	Box2DShapeInstance *shape;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction;
};

struct CastHitCollector {
	int max_results = 0;
	Box2DPhysicsDirectSpaceState2D *context;
	bool hit = false;
	Vector<CastHit> hits;
	CastHitCollector(int p_max_results, Box2DPhysicsDirectSpaceState2D *p_context) :
			max_results(p_max_results), context(p_context) {}
};

struct NearestCastHitCollector {
	CastHit nearest_hit;
	Box2DPhysicsDirectSpaceState2D *context;
	bool hit = false;
	NearestCastHitCollector(Box2DPhysicsDirectSpaceState2D *p_context) :
			context(p_context) {}
};

float nearest_raycast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	NearestCastHitCollector *collector = static_cast<NearestCastHitCollector *>(context);

	b2BodyId body_id = b2Shape_GetBody(shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shapeId));

	if (collector->context->is_body_excluded_from_query(body->get_rid())) {
		return -1;
	}

	collector->hit = true;
	collector->nearest_hit = CastHit{ body, shape, point, normal, fraction };

	return fraction;
}