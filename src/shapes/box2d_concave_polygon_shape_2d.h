#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DConcavePolygonShape2D : public Box2DShape2D {
public:
	ShapeIdAndGeometry add_to_body(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const override;
	ShapeGeometry get_shape_geometry(const Transform2D &p_transform) const override;
	PhysicsServer2D::ShapeType get_type() const override { return PhysicsServer2D::ShapeType::SHAPE_CONCAVE_POLYGON; }

	static bool is_polygon_clockwise(const PackedVector2Array &p_points);
};