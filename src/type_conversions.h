#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

_FORCE_INLINE_ Vector2 to_godot(const b2Vec2 p_vec) {
	return Vector2(p_vec.x, p_vec.y);
}

_FORCE_INLINE_ b2Vec2 to_box2d(const Vector2 p_vec) {
	return b2Vec2{ p_vec.x, p_vec.y };
}

_FORCE_INLINE_ b2Transform to_box2d(const Transform2D p_transform) {
	b2Transform transform;
	transform.p = to_box2d(p_transform.get_origin());
	transform.q = b2MakeRot(p_transform.get_rotation());
	return transform;
}