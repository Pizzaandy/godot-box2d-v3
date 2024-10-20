#include "box2d_rectangle_shape_2d.h"

ShapeID Box2DRectangleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2, {});

	Vector2 half_extents = data;

	b2Vec2 points[4] = {
		to_box2d(p_transform.xform(Vector2(-half_extents.x, half_extents.y))),
		to_box2d(p_transform.xform(Vector2(half_extents.x, half_extents.y))),
		to_box2d(p_transform.xform(Vector2(half_extents.x, -half_extents.y))),
		to_box2d(p_transform.xform(Vector2(-half_extents.x, -half_extents.y)))
	};

	b2Hull hull = b2ComputeHull(points, 4);

	b2Polygon box = b2MakePolygon(&hull, 0.0);

	return b2CreatePolygonShape(p_body, &p_shape_def, &box);
}