#pragma once

#include "../box2d_globals.h"
#include "box2d_physics_direct_space_state_2d.h"
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

class Box2DShapeInstance;
class Box2DCollisionObject2D;

class QueryFilter {
public:
	b2QueryFilter filter = b2DefaultQueryFilter();
	virtual bool is_excluded(const Box2DCollisionObject2D *p_object) const { return false; }
};

class BodyQueryFilter : public QueryFilter {
public:
	explicit BodyQueryFilter(b2BodyId p_body_id) :
			body_id(p_body_id) {}

	bool is_excluded(const Box2DCollisionObject2D *p_object) const override;

	b2BodyId body_id;
};

class SpaceStateQueryFilter : public QueryFilter {
public:
	explicit SpaceStateQueryFilter(Box2DDirectSpaceState2D *p_space_state, b2QueryFilter p_filter) {
		space_state = p_space_state;
		filter = p_filter;
	}

	bool is_excluded(const Box2DCollisionObject2D *p_object) const override;

	Box2DDirectSpaceState2D *space_state = nullptr;
};

class ArrayQueryFilter : public QueryFilter {
public:
	TypedArray<RID> exclude;

	explicit ArrayQueryFilter(TypedArray<RID> p_exclude, b2QueryFilter p_filter) {
		filter = p_filter;
		exclude = p_exclude;
	}

	bool is_excluded(const Box2DCollisionObject2D *p_object) const override;
};

struct OverlapQuery {
	b2WorldId world;
	QueryFilter filter;
	Transform2D transform = Transform2D();
	int max_results = 0;
};

struct CastQuery {
	b2WorldId world;
	QueryFilter filter;
	Transform2D transform;
	Vector2 translation;
	int max_results;
	bool find_nearest;
};

/// Overlap query result
struct ShapeOverlap {
	Box2DCollisionObject2D *object = nullptr;
	Box2DShapeInstance *shape = nullptr;
	b2ShapeId shape_id;
	ShapeCollideResult collision;

	bool operator==(const ShapeOverlap &p_other) const {
		return B2_ID_EQUALS(p_other.shape_id, shape_id);
	}
};

/// Cast query result
struct CastHit {
	Box2DCollisionObject2D *object;
	Box2DShapeInstance *shape;
	b2ShapeId shape_id;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction;

	bool operator<(const CastHit &p_other) const {
		return fraction < p_other.fraction;
	}

	bool operator==(const CastHit &p_other) const {
		return B2_ID_EQUALS(p_other.shape_id, shape_id);
	}
};

struct CastQueryCollector {
	int max_results = 0;
	int count = 0;
	bool find_nearest = false;
	const QueryFilter filter;
	LocalVector<CastHit> &results;

	explicit CastQueryCollector(const CastQuery &p_query, LocalVector<CastHit> &p_results) :
			results(p_results), max_results(p_query.max_results), filter(p_query.filter), find_nearest(p_query.find_nearest) {}

	explicit CastQueryCollector(int p_max_results, const QueryFilter p_filter, bool p_find_nearest, LocalVector<CastHit> &p_results) :
			results(p_results), max_results(p_max_results), filter(p_filter), find_nearest(p_find_nearest) {}
};

struct OverlapQueryCollector {
	int max_results = 0;
	int count = 0;
	const QueryFilter filter;
	LocalVector<ShapeOverlap> &results;

	explicit OverlapQueryCollector(const OverlapQuery &p_query, LocalVector<ShapeOverlap> &p_results) :
			results(p_results), max_results(p_query.max_results), filter(p_query.filter) {}

	explicit OverlapQueryCollector(int p_max_results, const QueryFilter p_filter, LocalVector<ShapeOverlap> &p_results) :
			results(p_results), max_results(p_max_results), filter(p_filter) {}
};

bool overlap_callback(b2ShapeId shapeId, void *context);

float cast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);

int find_nearest_cast_hit(LocalVector<CastHit> &p_results);