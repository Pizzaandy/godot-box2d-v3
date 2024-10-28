#pragma once

#include "box2d_shape_2d.h"

class Box2DShape2D;

class Box2DShapeInstance {
public:
	int index = -1;
	ShapeID shape_id;
	ShapeGeometry shape_info;
	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 0.0;

	void set_shape(Box2DShape2D *p_shape);

	Box2DShape2D *get_shape_or_null() const { return shape; }

	void build(b2BodyId p_body, const Transform2D &p_transform, b2ShapeDef p_shape_def);

	void destroy();

	~Box2DShapeInstance() {
		destroy();
		set_shape(nullptr);
	}

private:
	Box2DShape2D *shape = nullptr;
};