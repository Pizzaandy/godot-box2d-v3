#include "box2d_rectangle_shape_2d.h"

ShapeIdAndGeometry Box2DRectangleShape2D::add_to_body(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	ShapeIdAndGeometry result;

	result.info = get_shape_info(p_transform);
	if (!result.info.is_valid()) {
		return result;
	}

	result.id = b2CreatePolygonShape(p_body, &p_shape_def, &result.info.polygon);
	return result;
}

ShapeGeometry Box2DRectangleShape2D::get_shape_info(const Transform2D &p_transform) const {
	ShapeGeometry shape;
	shape.type = ShapeGeometry::Type::POLYGON;
	if (!make_rectangle(p_transform, data, shape.polygon)) {
		return ShapeGeometry::invalid();
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