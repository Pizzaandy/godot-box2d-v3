#include "box2d_circle_shape_2d.h"

b2ShapeId Box2DCircleShape2D::build(b2BodyId p_body_id, Transform2D p_transform, bool p_disabled, b2ShapeId p_shape_id) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::FLOAT && type != Variant::INT, b2_nullShapeId);

	float radius = data;
	Vector2 origin = p_transform.get_origin();
	Size2 size = p_transform.get_scale();
	radius *= size.x;

	b2Circle circle;
	circle.center = b2Vec2{ origin.x, origin.y };
	circle.radius = radius;

	if (B2_IS_NON_NULL(p_shape_id)) {
		b2Shape_SetCircle(p_shape_id, &circle);
		return p_shape_id;
	}

	b2ShapeDef shape_def = b2DefaultShapeDef();
	return b2CreateCircleShape(p_body_id, &shape_def, &circle);
}