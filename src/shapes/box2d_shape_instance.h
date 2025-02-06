#pragma once

#include "box2d_shape_2d.h"
#include <godot_cpp/templates/local_vector.hpp>

class Box2DShape2D;
class Box2DCollisionObject2D;

class Box2DShapeInstance {
public:
	explicit Box2DShapeInstance(Box2DCollisionObject2D *p_body,
			Box2DShape2D *p_shape,
			const Transform2D &p_transform,
			bool p_disabled);

	Box2DShapeInstance() = default;

	~Box2DShapeInstance();

	int index = -1;
	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 0.0;

	LocalVector<b2ShapeId> shape_ids;

	void set_shape(Box2DShape2D *p_shape) { shape = p_shape; }
	Box2DShape2D *get_shape_or_null() const { return shape; }

	Transform2D get_shape_transform() const;
	b2ShapeDef get_shape_def() const;

	void build();

private:
	Box2DCollisionObject2D *body = nullptr;
	Box2DShape2D *shape = nullptr;
};