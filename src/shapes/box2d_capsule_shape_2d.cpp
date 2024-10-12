#include "box2d_capsule_shape_2d.h"

b2ShapeId Box2DCapsuleShape2D::build(b2BodyId p_body_id, Transform2D p_transform, bool p_disabled, b2ShapeId p_shape_id) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2 && type != Variant::ARRAY, b2_nullShapeId);

	float radius, height;

	if (type == Variant::ARRAY) {
		Array arr = data;
		ERR_FAIL_COND_V(arr.size() != 2, b2_nullShapeId);
		height = arr[0];
		radius = arr[1];
	} else {
		Vector2 p = data;
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

	b2Capsule capsule;
	capsule.center1 = b2Vec2{ center1.x, center1.y };
	capsule.center2 = b2Vec2{ center2.x, center2.y };
	capsule.radius = radius;

	if (B2_IS_NON_NULL(p_shape_id)) {
		b2Shape_SetCapsule(p_shape_id, &capsule);
		return p_shape_id;
	}

	b2ShapeDef shape_def = b2DefaultShapeDef();
	return b2CreateCapsuleShape(p_body_id, &shape_def, &capsule);
}