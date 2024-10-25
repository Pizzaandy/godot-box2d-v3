#include "box2d_rectangle_shape_2d.h"

ShapeID Box2DRectangleShape2D::build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	b2Polygon box;

	if (!make_rectangle(p_transform, data, box)) {
		return ShapeID::invalid();
	}

	return b2CreatePolygonShape(p_body, &p_shape_def, &box);
}

ShapeInfo Box2DRectangleShape2D::get_shape_info(const Transform2D &p_transform) const {
	ShapeInfo shape;
	shape.type = ShapeInfo::Type::POLYGON;
	if (!make_rectangle(p_transform, data, shape.polygon)) {
		return ShapeInfo::invalid();
	}
	return shape;
}

bool Box2DRectangleShape2D::make_rectangle(const Transform2D &p_transform, const Variant &p_data, b2Polygon &p_box) {
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