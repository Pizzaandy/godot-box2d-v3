#include "box2d_capsule_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DCapsuleShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Capsule shape;
	if (!make_capsule(p_instance->get_shape_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreateCapsuleShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

ShapeGeometry Box2DCapsuleShape2D::get_shape_geometry(const Transform2D &p_transform) const {
	ShapeGeometry shape;
	shape.type = ShapeGeometry::Type::CAPSULE;
	if (!make_capsule(p_transform, data, shape.capsule)) {
		return ShapeGeometry::invalid();
	}
	return shape;
}

bool Box2DCapsuleShape2D::make_capsule(const Transform2D &p_transform, const Variant &p_data, b2Capsule &p_capsule) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::VECTOR2 && type != Variant::ARRAY, false);

	float radius, height;

	if (type == Variant::ARRAY) {
		Array arr = p_data;
		ERR_FAIL_COND_V(arr.size() != 2, false);
		height = arr[0];
		radius = arr[1];
	} else {
		Vector2 p = p_data;
		// Order for Vector2 is flipped for some reason
		radius = p.x;
		height = p.y;
	}

	Vector2 origin = p_transform.get_origin();
	Vector2 capsule_up = p_transform.columns[1].normalized();
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