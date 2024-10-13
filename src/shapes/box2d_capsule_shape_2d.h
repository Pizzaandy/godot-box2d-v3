#pragma once

#include "box2d/box2d.h"
#include "box2d_shape_2d.h"

class Box2DCapsuleShape2D : public Box2DShape2D {
public:
	b2ShapeId build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) override;
	void update(b2ShapeId p_shape, Transform2D p_transform) override;
	b2Capsule make_capsule(Transform2D p_transform);
};