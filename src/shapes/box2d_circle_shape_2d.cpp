#include "box2d_circle_shape_2d.h"

Box2DShape2D::ShapeID Box2DCircleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::FLOAT && type != Variant::INT, b2_nullShapeId);

	float radius = data;
	Vector2 origin = p_transform.get_origin();
	Size2 size = p_transform.get_scale();
	radius *= size.x;

	b2Circle circle;
	circle.center = to_box2d(origin);
	circle.radius = to_box2d(radius);

	return b2CreateCircleShape(p_body, &p_shape_def, &circle);
}