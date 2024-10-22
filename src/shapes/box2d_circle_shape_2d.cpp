#include "box2d_circle_shape_2d.h"

ShapeID Box2DCircleShape2D::build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	b2Circle circle;

	if (!make_circle(p_transform, data, circle)) {
		return {};
	}

	return b2CreateCircleShape(p_body, &p_shape_def, &circle);
}

void Box2DCircleShape2D::cast_shape(
		b2WorldId p_world,
		const Transform2D &p_transform,
		Vector2 p_motion,
		b2QueryFilter p_filter,
		b2CastResultFcn *fcn,
		void *context) const {
	b2Circle circle;

	if (!make_circle(p_transform, data, circle)) {
		return;
	}

	b2World_CastCircle(p_world, &circle, b2Transform_identity, to_box2d(p_motion), p_filter, fcn, context);
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