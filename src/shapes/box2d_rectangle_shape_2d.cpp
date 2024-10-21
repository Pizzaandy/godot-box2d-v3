#include "box2d_rectangle_shape_2d.h"

ShapeID Box2DRectangleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	b2Polygon box;

	if (!make_rectangle(p_transform, data, box)) {
		return {};
	}

	return b2CreatePolygonShape(p_body, &p_shape_def, &box);
}

void Box2DRectangleShape2D::cast_shape(
		b2WorldId p_world,
		Transform2D p_transform,
		Vector2 p_motion,
		b2QueryFilter p_filter,
		b2CastResultFcn *fcn,
		void *context) {
	b2Polygon box;

	if (!make_rectangle(p_transform, data, box)) {
		return;
	}

	b2World_CastPolygon(p_world, &box, b2Transform_identity, to_box2d(p_motion), p_filter, fcn, context);
}

bool Box2DRectangleShape2D::make_rectangle(Transform2D p_transform, Variant p_data, b2Polygon &p_box) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2, false);

	Vector2 half_extents = p_data;

	b2Vec2 points[4] = {
		to_box2d(p_transform.xform(Vector2(-half_extents.x, half_extents.y))),
		to_box2d(p_transform.xform(Vector2(half_extents.x, half_extents.y))),
		to_box2d(p_transform.xform(Vector2(half_extents.x, -half_extents.y))),
		to_box2d(p_transform.xform(Vector2(-half_extents.x, -half_extents.y)))
	};

	b2Hull hull = b2ComputeHull(points, 4);

	p_box = b2MakePolygon(&hull, 0.0);

	return true;
}