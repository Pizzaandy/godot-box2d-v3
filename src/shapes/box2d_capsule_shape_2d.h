#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DCapsuleShape2D : public Box2DShape2D {
public:
	ShapeID build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const override;
	void cast_shape(b2WorldId p_world, const Transform2D &p_transform, Vector2 p_motion, b2QueryFilter p_filter, b2CastResultFcn *fcn, void *context) const override;
	static bool make_capsule(const Transform2D &p_transform, const Variant &p_data, b2Capsule &p_capsule);
};