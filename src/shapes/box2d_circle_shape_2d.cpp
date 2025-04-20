#include "box2d_circle_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DCircleShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Circle shape;
	if (!make_circle(p_instance->get_global_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreateCircleShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

int Box2DCircleShape2D::cast(const CastQuery &p_query, const Transform2D &p_transform, LocalVector<CastHit> &p_results) const {
	b2Circle shape;
	if (!make_circle(p_transform, data, shape)) {
		return 0;
	}
	return box2d_cast_shape(shape, p_query, p_results);
}

int Box2DCircleShape2D::overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const {
	b2Circle shape;
	if (!make_circle(p_transform, data, shape)) {
		return 0;
	}
	return box2d_overlap_shape(shape, p_query, p_results);
}

bool Box2DCircleShape2D::make_circle(const Transform2D &p_transform, const Variant &p_data, b2Circle &p_circle) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::FLOAT && type != Variant::INT, false);

	float radius = p_data;
	Vector2 origin = p_transform.get_origin();
	radius *= p_transform.get_scale().x;

	p_circle.center = to_box2d(origin);
	p_circle.radius = to_box2d(radius);

	return true;
}