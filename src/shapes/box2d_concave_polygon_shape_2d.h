#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DConcavePolygonShape2D : public Box2DShape2D {
public:
	ShapeID build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) override;
	void cast_shape(b2WorldId p_world, Transform2D p_transform, Vector2 p_motion, b2QueryFilter p_filter, b2CastResultFcn *fcn, void *context) override;
	bool is_polygon_clockwise(const PackedVector2Array &p_points);
};