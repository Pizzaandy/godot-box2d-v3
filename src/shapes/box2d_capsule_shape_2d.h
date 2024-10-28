#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DCapsuleShape2D : public Box2DShape2D {
public:
	ShapeIdAndGeometry add_to_body(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const override;
	ShapeGeometry get_shape_info(const Transform2D &p_transform) const override;

	static bool make_capsule(const Transform2D &p_transform, const Variant &p_data, b2Capsule &p_capsule);
};