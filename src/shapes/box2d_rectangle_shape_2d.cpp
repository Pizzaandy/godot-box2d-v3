#include "box2d_rectangle_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DRectangleShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Polygon shape;
	if (!make_rectangle(p_instance->get_shape_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreatePolygonShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

int Box2DRectangleShape2D::cast(const CastQuery &p_query, LocalVector<CastHit> &p_results) const {
	b2Polygon shape;
	if (!make_rectangle(p_query.origin, data, shape)) {
		return 0;
	}
	CastQueryCollector collector(p_query.max_results, p_query.filter, p_results, p_query.find_nearest);
	b2World_CastPolygon(p_query.world, &shape, to_box2d(p_query.translation), p_query.filter.filter, cast_callback, &collector);
	return collector.results.size();
}

int Box2DRectangleShape2D::overlap(const OverlapQuery &p_query, LocalVector<ShapeOverlap> &p_results) const {
	b2Polygon shape;
	if (!make_rectangle(p_query.origin, data, shape)) {
		return 0;
	}
	OverlapQueryCollector collector(p_query.max_results, p_query.filter, p_results);
	b2World_OverlapPolygon(p_query.world, &shape, b2Transform_identity, p_query.filter.filter, overlap_callback, &collector);
	return collector.results.size();
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