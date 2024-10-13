#include "box2d_rectangle_shape_2d.h"

b2ShapeId Box2DRectangleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	b2Polygon box = make_box(p_transform);
	return b2CreatePolygonShape(p_body, &p_shape_def, &box);
}

void Box2DRectangleShape2D::update(b2ShapeId p_shape, Transform2D p_transform) {
	b2Polygon box = make_box(p_transform);
	b2Shape_SetPolygon(p_shape, &box);
}

b2Polygon Box2DRectangleShape2D::make_box(Transform2D p_transform) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2, b2Polygon());

	Vector2 half_extents = data;

	Vector2 origin = p_transform.get_origin();
	float angle = p_transform.get_rotation();
	half_extents *= p_transform.get_scale().x;

	return b2MakeOffsetBox(half_extents.x, half_extents.y, b2Vec2{ origin.x, origin.y }, b2MakeRot(angle));
}