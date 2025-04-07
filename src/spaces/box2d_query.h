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
	Transform2D origin;
	int max_results;
};

struct CastQuery {
	b2WorldId world;
	QueryFilter filter;
	Transform2D origin;
	Vector2 translation;
	int max_results;
	bool find_nearest;
};

/// Overlap query result
struct ShapeOverlap {
	Box2DCollisionObject2D *object = nullptr;
	Box2DShapeInstance *shape = nullptr;
	b2ShapeId shape_id;
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
};

struct CastQueryCollector {
	int max_results = 0;
	bool find_nearest = false;
	const QueryFilter filter;
	LocalVector<CastHit> &results;

	explicit CastQueryCollector(int p_max_results, const QueryFilter &p_filter, LocalVector<CastHit> &p_results, bool p_find_nearest) :
			max_results(p_max_results), filter(p_filter), results(p_results), find_nearest(p_find_nearest) {
		p_results.clear();
	}
};

struct OverlapQueryCollector {
	int max_results = 0;
	const QueryFilter filter;
	LocalVector<ShapeOverlap> &results;

	explicit OverlapQueryCollector(int p_max_results, const QueryFilter &p_filter, LocalVector<ShapeOverlap> &p_results) :
			max_results(p_max_results), filter(p_filter), results(p_results) {
		p_results.clear();
	}
};

bool overlap_callback(b2ShapeId shapeId, void *context);

float cast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);