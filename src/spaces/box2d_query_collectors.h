#pragma once

#include "box2d_physics_direct_space_state_2d.h"

class Box2DBody2D;
class Box2DShapeInstance;

class QueryFilter {
public:
	virtual bool is_excluded(const RID &p_body) { return false; }
};

class SpaceStateQueryFilter : public QueryFilter {
public:
	bool is_excluded(const RID &p_body) override {
		return space_state->is_body_excluded_from_query(p_body);
	}

	SpaceStateQueryFilter(Box2DDirectSpaceState2D *p_space_state) :
			space_state(p_space_state) {}

private:
	Box2DDirectSpaceState2D *space_state = nullptr;
};

class ArrayQueryFilter : public QueryFilter {
public:
	bool is_excluded(const RID &p_body) override {
		return exclude->has(p_body);
	}

	ArrayQueryFilter(TypedArray<RID> *p_exclude) :
			exclude(p_exclude) {}

private:
	TypedArray<RID> *exclude;
};

/// Overlap query result
struct ShapeOverlap {
	Box2DBody2D *body = nullptr;
	Box2DShapeInstance *shape = nullptr;
	b2ShapeId shape_id;
};

/// Cast query result
struct CastHit {
	Box2DBody2D *body;
	Box2DShapeInstance *shape;
	b2ShapeId shape_id;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction;
};

struct ShapeOverlapCollector {
	int max_results = 0;
	Vector<ShapeOverlap> overlaps;
	QueryFilter *filter;

	ShapeOverlapCollector(int p_max_results, QueryFilter *p_filter) :
			filter(p_filter),
			max_results(p_max_results) {}

	bool contains_id(const b2ShapeId &id) {
		for (ShapeOverlap overlap : overlaps) {
			if (B2_ID_EQUALS(overlap.shape_id, id)) {
				return true;
			}
		}
		return false;
	}
};

struct NearestCastHitCollector {
	CastHit nearest_hit;
	QueryFilter *filter;
	bool hit = false;
	NearestCastHitCollector(QueryFilter *p_filter) :
			filter(p_filter) {}
};

struct CastHitCollector {
	int max_results = 0;
	QueryFilter *filter;
	bool hit = false;
	Vector<CastHit> hits;

	CastHitCollector(int p_max_results, QueryFilter *p_filter) :
			max_results(p_max_results), filter(p_filter) {}

	bool contains_id(const b2ShapeId &id) {
		for (CastHit hit : hits) {
			if (B2_ID_EQUALS(hit.shape_id, id)) {
				return true;
			}
		}
		return false;
	}
};

bool overlap_callback(b2ShapeId shapeId, void *context);

float cast_callback_nearest(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);

float cast_callback_all(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);