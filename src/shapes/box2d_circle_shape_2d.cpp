#include "box2d_circle_shape_2d.h"

b2ShapeId Box2DCircleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	b2Circle circle = make_circle(p_transform);
	return b2CreateCircleShape(p_body, &p_shape_def, &circle);
}

void Box2DCircleShape2D::update(b2ShapeId p_shape, Transform2D p_transform) {
	b2Circle circle = make_circle(p_transform);
	b2Shape_SetCircle(p_shape, &circle);
}

b2Circle Box2DCircleShape2D::make_circle(Transform2D p_transform) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::FLOAT && type != Variant::INT, {});

	float radius = data;
	Vector2 origin = p_transform.get_origin();
	Size2 size = p_transform.get_scale();
	radius *= size.x;

	b2Circle circle;
	circle.center = b2Vec2{ origin.x, origin.y };
	circle.radius = radius;

	return circle;
}