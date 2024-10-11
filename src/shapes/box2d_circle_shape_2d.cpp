#include "box2d_circle_shape_2d.h"

b2ShapeId Box2DCircleShape2D::build(b2BodyId p_body_id, Transform2D p_transform, bool p_disabled, b2ShapeId p_shape_id) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::Type::FLOAT && type != Variant::Type::INT, b2_nullShapeId);

	float radius = data;
	Vector2 origin = p_transform.get_origin();
	Size2 size = p_transform.get_scale();
	radius *= Math::min(size.x, size.y);

	b2Circle circle;
	circle.center = b2Vec2{ origin.x, origin.y };
	circle.radius = radius;

	b2ShapeDef shape_def = b2DefaultShapeDef();

	if (B2_IS_NON_NULL(p_shape_id)) {
		b2DestroyShape(p_shape_id, false);
	}

	return b2CreateCircleShape(p_body_id, &shape_def, &circle);
}