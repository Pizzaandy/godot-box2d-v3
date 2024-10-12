#include "box2d_rectangle_shape_2d.h"

b2ShapeId Box2DRectangleShape2D::build(b2BodyId p_body_id, Transform2D p_transform, bool p_disabled, b2ShapeId p_shape_id) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2, b2_nullShapeId);

	Vector2 half_extents = data;

	Vector2 origin = p_transform.get_origin();
	float angle = p_transform.get_rotation();
	half_extents *= p_transform.get_scale().x;

	b2Polygon box = b2MakeOffsetBox(half_extents.x, half_extents.y, b2Vec2{ origin.x, origin.y }, b2MakeRot(angle));

	if (B2_IS_NON_NULL(p_shape_id)) {
		b2Shape_SetPolygon(p_shape_id, &box);
		return p_shape_id;
	}

	b2ShapeDef shape_def = b2DefaultShapeDef();
	return b2CreatePolygonShape(p_body_id, &shape_def, &box);
}