#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

extern float BOX2D_PIXELS_PER_METER;

/// @brief This mask bit is used by queries only.
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