#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DRectangleShape2D : public Box2DShape2D {
public:
	ShapeID build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const override;
	ShapeInfo get_shape_info(const Transform2D &p_transform) const override;

	static bool make_rectangle(const Transform2D &p_transform, const Variant &p_data, b2Polygon &p_box);
};