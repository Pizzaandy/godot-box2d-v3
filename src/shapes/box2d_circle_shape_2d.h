#pragma once

#include "box2d/box2d.h"
#include "box2d_shape_2d.h"

class Box2DCircleShape2D : public Box2DShape2D {
public:
	ShapeID build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) override;
};