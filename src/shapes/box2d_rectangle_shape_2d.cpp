#include "box2d_rectangle_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DRectangleShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Polygon shape;
	if (!make_rectangle(p_instance->get_global_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreatePolygonShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

int Box2DRectangleShape2D::cast(const CastQuery &p_query, const Transform2D &p_transform, LocalVector<CastHit> &p_results) const {
	b2Polygon shape;
	if (!make_rectangle(p_transform, data, shape)) {
		return 0;
	}
	return box2d_cast_shape(shape, p_query, p_results);
}

int Box2DRectangleShape2D::overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const {
	b2Polygon shape;
	if (!make_rectangle(p_transform, data, shape)) {
		return 0;
	}
	return box2d_overlap_shape(shape, p_query, p_results);
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