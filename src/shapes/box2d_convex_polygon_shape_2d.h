#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DConvexPolygonShape2D : public Box2DShape2D {
public:
	void add_to_body(Box2DShapeInstance *p_instance) const override;

	ShapeGeometry get_shape_geometry(const Transform2D &p_transform) const override;
	PhysicsServer2D::ShapeType get_type() const override { return PhysicsServer2D::ShapeType::SHAPE_CONVEX_POLYGON; }

	static bool make_polygon(const Transform2D &p_transform, const Variant &p_data, b2Polygon &p_polygon);
};