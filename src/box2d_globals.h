#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

extern float BOX2D_PIXELS_PER_METER;

/// This mask bit is used by queries only.
const uint64_t COMMON_MASK_BIT = 0x8000000000000000ULL;

_FORCE_INLINE_ void box2d_set_pixels_per_meter(float p_value) {
	BOX2D_PIXELS_PER_METER = p_value;
}

_FORCE_INLINE_ Vector2 to_godot(const b2Vec2 p_vec) {
	float scale = BOX2D_PIXELS_PER_METER;
	return Vector2(scale * p_vec.x, scale * p_vec.y);
}

_FORCE_INLINE_ b2Vec2 to_box2d(const Vector2 p_vec) {
	float scale = 1 / BOX2D_PIXELS_PER_METER;
	return scale * b2Vec2{ p_vec.x, p_vec.y };
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

struct ShapeInfo {
	bool is_valid = true;
	b2ShapeType type;
	union {
		b2Capsule capsule;
		b2Circle circle;
		b2Polygon polygon;
		b2Segment segment;
		b2ChainSegment chainSegment;
	};

	static ShapeInfo invalid() {
		return ShapeInfo{ false };
	}
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
		const ShapeInfo &p_shape_a,
		const b2Transform &xfa,
		const ShapeInfo &p_shape_b,
		const b2Transform &xf,
		bool p_swapped = false);

void box2d_cast_shape(
		const b2WorldId &p_world,
		const ShapeInfo &p_shape,
		const b2Transform &p_transform,
		const b2Vec2 &p_motion,
		const b2QueryFilter &p_filter,
		b2CastResultFcn *fcn,
		void *context);

void box2d_overlap_shape(
		const b2WorldId &p_world,
		const ShapeInfo &p_shape,
		const b2Transform &p_transform,
		const b2QueryFilter &p_filter,
		b2OverlapResultFcn *fcn,
		void *context);