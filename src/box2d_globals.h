#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

#ifdef TRACY_ENABLE
#include "../tracy/public/tracy/Tracy.hpp"
#define TracyZoneScoped(name) ZoneScopedN(name)
#else
#define TracyZoneScoped(name)
#endif

using namespace godot;

extern float BOX2D_PIXELS_PER_METER;

/// Mask bit used by all bodies and areas.
/// This allows queries to find objects with a mask of 0 (consistent with Godot physics).
const uint64_t COMMON_MASK_BIT = 1ULL << 63;

_FORCE_INLINE_ void box2d_set_pixels_per_meter(float p_value) {
	BOX2D_PIXELS_PER_METER = p_value;
}

_FORCE_INLINE_ Vector2 to_godot(const b2Vec2 p_vec) {
	float scale = BOX2D_PIXELS_PER_METER;
	return Vector2(scale * p_vec.x, scale * p_vec.y);
}

_FORCE_INLINE_ b2Vec2 to_box2d(const Vector2 p_vec) {
	float scale = 1 / BOX2D_PIXELS_PER_METER;
	return scale * b2Vec2{ (float)p_vec.x, (float)p_vec.y };
}

_FORCE_INLINE_ float to_box2d(float p_length) {
	float scale = 1 / BOX2D_PIXELS_PER_METER;
	return scale * p_length;
}

_FORCE_INLINE_ float to_godot(float p_length) {
	float scale = BOX2D_PIXELS_PER_METER;
	return scale * p_length;
}

_FORCE_INLINE_ b2Transform to_box2d(Transform2D p_transform) {
	return b2Transform{ to_box2d(p_transform.get_origin()), b2MakeRot(p_transform.get_rotation()) };
}

/// Represents a Box2D shape geometry type.
struct ShapeGeometry {
	enum Type {
		INVALID,
		CIRCLE,
		CAPSULE,
		SEGMENT,
		POLYGON,
		CHAIN_SEGMENT,
	};

	Type type = Type::INVALID;

	union {
		b2Capsule capsule;
		b2Circle circle;
		b2Polygon polygon;
		b2Segment segment;
		b2ChainSegment chain_segment;
	};

	_FORCE_INLINE_ bool is_valid() {
		return type != Type::INVALID;
	}

	_FORCE_INLINE_ static ShapeGeometry invalid() {
		return {};
	}
};

/// Range for iterating body shapes.
class BodyShapeRange {
public:
	explicit BodyShapeRange(b2BodyId body_id) :
			body_id(body_id) {
		shape_count = b2Body_GetShapeCount(body_id);
		shape_ids = memnew_arr(b2ShapeId, shape_count);
		b2Body_GetShapes(body_id, shape_ids, shape_count);
	}

	~BodyShapeRange() {
		memdelete_arr(shape_ids);
	}

	class Iterator {
	public:
		Iterator(b2ShapeId *ids, int index) :
				shape_ids(ids), index(index) {}

		b2ShapeId operator*() const {
			return shape_ids[index];
		}

		Iterator &operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator &other) const {
			return index != other.index;
		}

	private:
		b2ShapeId *shape_ids;
		int index;
	};

	Iterator begin() const {
		return Iterator(shape_ids, 0);
	}

	Iterator end() const {
		return Iterator(shape_ids, shape_count);
	}

private:
	b2BodyId body_id;
	b2ShapeId *shape_ids;
	int shape_count;
};

/// Range for iterating chain segment shapes.
class ChainSegmentRange {
public:
	explicit ChainSegmentRange(b2ChainId chain_id) :
			chain_id(chain_id) {
		segment_count = b2Chain_GetSegmentCount(chain_id);
		shape_ids = memnew_arr(b2ShapeId, segment_count);
		b2Chain_GetSegments(chain_id, shape_ids, segment_count);
	}

	~ChainSegmentRange() {
		memdelete_arr(shape_ids);
	}

	class Iterator {
	public:
		Iterator(b2ShapeId *ids, int index) :
				shape_ids(ids), index(index) {}

		b2ShapeId operator*() const {
			return shape_ids[index];
		}

		Iterator &operator++() {
			++index;
			return *this;
		}

		bool operator!=(const Iterator &other) const {
			return index != other.index;
		}

	private:
		b2ShapeId *shape_ids;
		int index;
	};

	Iterator begin() const {
		return Iterator(shape_ids, 0);
	}

	Iterator end() const {
		return Iterator(shape_ids, segment_count);
	}

private:
	b2ChainId chain_id;
	b2ShapeId *shape_ids;
	int segment_count;
};

struct ShapeCollidePoint {
	Vector2 point;
	float separation;
};

struct ShapeCollideResult {
	ShapeCollidePoint points[2];
	Vector2 normal = {};
	int32_t point_count = 0;
};

ShapeCollideResult box2d_collide_shapes(
		const ShapeGeometry &p_shape_a,
		const b2Transform &xfa,
		const ShapeGeometry &p_shape_b,
		const b2Transform &xf,
		bool p_swapped = false);

void box2d_cast_shape(
		const b2WorldId &p_world,
		const ShapeGeometry &p_shape,
		const b2Transform &p_transform,
		const b2Vec2 &p_motion,
		const b2QueryFilter &p_filter,
		b2CastResultFcn *fcn,
		void *context);

void box2d_overlap_shape(
		const b2WorldId &p_world,
		const ShapeGeometry &p_shape,
		const b2Transform &p_transform,
		const b2QueryFilter &p_filter,
		b2OverlapResultFcn *fcn,
		void *context);

ShapeGeometry get_shape_geometry(const b2ShapeId &p_shape);