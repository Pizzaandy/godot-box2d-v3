#pragma once

#include "box2d_shape_2d.h"

class Box2DShape2D;
class Box2DCollisionObject2D;

class Box2DShapeInstance {
public:
	explicit Box2DShapeInstance(Box2DCollisionObject2D *p_body,
			Box2DShape2D *p_shape,
			const Transform2D &p_transform,
			bool p_disabled) :
			body(p_body),
			shape(p_shape),
			transform(p_transform),
			disabled(p_disabled) {
		if (p_shape) {
			p_shape->add_instance(this);
		}
	}

	~Box2DShapeInstance() {
		if (shape) {
			shape->remove_instance(this);
		}
	}

	int index = -1;
	b2ShapeDef shape_def;

	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 0.0;

	void set_shape(Box2DShape2D *p_shape);
	Box2DShape2D *get_shape_or_null() const { return shape; }

	void build();

private:
	Box2DCollisionObject2D *body = nullptr;
	Box2DShape2D *shape = nullptr;
};