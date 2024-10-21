#include "box2d_capsule_shape_2d.h"

ShapeID Box2DCapsuleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	b2Capsule capsule;

	if (!make_capsule(p_transform, data, capsule)) {
		return {};
	}

	return b2CreateCapsuleShape(p_body, &p_shape_def, &capsule);
}

void Box2DCapsuleShape2D::cast_shape(
		b2WorldId p_world,
		Transform2D p_transform,
		Vector2 p_motion,
		b2QueryFilter p_filter,
		b2CastResultFcn *fcn,
		void *context) {
	b2Capsule capsule;

	if (!make_capsule(p_transform, data, capsule)) {
		return;
	}

	b2World_CastCapsule(p_world, &capsule, b2Transform_identity, to_box2d(p_motion), p_filter, fcn, context);
}

bool Box2DCapsuleShape2D::make_capsule(Transform2D p_transform, Variant p_data, b2Capsule &p_capsule) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2 && type != Variant::ARRAY, false);

	float radius, height;

	if (type == Variant::ARRAY) {
		Array arr = p_data;
		ERR_FAIL_COND_V(arr.size() != 2, {});
		height = arr[0];
		radius = arr[1];
	} else {
		Vector2 p = p_data;
		// ???
		radius = p.x;
		height = p.y;
	}

	Vector2 origin = p_transform.get_origin();
	Vector2 capsule_up = Vector2(0, 1).rotated(p_transform.get_rotation());
	float scale = p_transform.get_scale().x;

	radius *= scale;
	height *= scale;

	Vector2 center1 = origin + capsule_up * (0.5 * height - radius);
	Vector2 center2 = origin - capsule_up * (0.5 * height - radius);

	p_capsule.center1 = to_box2d(center1);
	p_capsule.center2 = to_box2d(center2);
	p_capsule.radius = to_box2d(radius);

	return true;
}