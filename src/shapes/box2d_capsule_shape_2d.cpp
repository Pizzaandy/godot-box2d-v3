#include "box2d_capsule_shape_2d.h"

ShapeID Box2DCapsuleShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2 && type != Variant::ARRAY, b2_nullShapeId);

	float radius, height;

	if (type == Variant::ARRAY) {
		Array arr = data;
		ERR_FAIL_COND_V(arr.size() != 2, {});
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
	capsule.center1 = to_box2d(center1);
	capsule.center2 = to_box2d(center2);
	capsule.radius = to_box2d(radius);

	return b2CreateCapsuleShape(p_body, &p_shape_def, &capsule);
}