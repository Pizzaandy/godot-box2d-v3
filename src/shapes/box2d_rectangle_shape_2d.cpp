#include "box2d_rectangle_shape_2d.h"

void Box2DRectangleShape2D::add_to_body(b2BodyId p_body_id, Box2DShapeInstance *p_instance) const {
	b2Polygon shape;
	if (!make_rectangle(p_instance->get_shape_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreatePolygonShape(p_body_id, &shape_def, &shape);
	p_instance->shape_ids.push_back(id);
}

ShapeGeometry Box2DRectangleShape2D::get_shape_geometry(const Transform2D &p_transform) const {
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