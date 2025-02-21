#include "box2d_circle_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DCircleShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Circle shape;
	if (!make_circle(p_instance->get_shape_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreateCircleShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

ShapeGeometry Box2DCircleShape2D::get_shape_geometry(const Transform2D &p_transform) const {
	ShapeGeometry shape;
	shape.type = ShapeGeometry::Type::CIRCLE;
	if (!make_circle(p_transform, data, shape.circle)) {
		return ShapeGeometry::invalid();
	}
	return shape;
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