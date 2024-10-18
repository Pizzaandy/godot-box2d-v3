#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

extern float box2d_pixels_per_meter;

_FORCE_INLINE_ void box2d_set_pixels_per_meter(float p_value) {
	box2d_pixels_per_meter = p_value;
}

_FORCE_INLINE_ Vector2 to_godot(const b2Vec2 p_vec) {
	float scale = box2d_pixels_per_meter;
	return Vector2(scale * p_vec.x, scale * p_vec.y);
}

_FORCE_INLINE_ b2Vec2 to_box2d(const Vector2 p_vec) {
	float scale = 1 / box2d_pixels_per_meter;
	return scale * b2Vec2{ p_vec.x, p_vec.y };
}

_FORCE_INLINE_ float to_box2d(float p_length) {
	float scale = 1 / box2d_pixels_per_meter;
	return scale * p_length;
}