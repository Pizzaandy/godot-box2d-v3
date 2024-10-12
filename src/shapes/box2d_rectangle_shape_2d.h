#pragma once

#include "box2d/box2d.h"
#include "box2d_shape_2d.h"

class Box2DRectangleShape2D : public Box2DShape2D {
public:
	b2ShapeId build(b2BodyId p_body_id, Transform2D p_shape_transform, bool p_disabled, b2ShapeId p_shape_id = b2_nullShapeId) override;
};