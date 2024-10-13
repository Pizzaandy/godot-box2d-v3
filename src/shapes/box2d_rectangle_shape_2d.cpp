#include "box2d_rectangle_shape_2d.h"

Box2DShape2D::ShapeID Box2DRectangleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2, {});

	Vector2 half_extents = data;

	Vector2 origin = p_transform.get_origin();
	float angle = p_transform.get_rotation();
	half_extents *= p_transform.get_scale().x;

	b2Polygon box = b2MakeOffsetBox(half_extents.x, half_extents.y, b2Vec2{ origin.x, origin.y }, b2MakeRot(angle));

	return b2CreatePolygonShape(p_body, &p_shape_def, &box);
}