#include "box2d_separation_ray_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DSeparationRayShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	// This shape only exists in character movement queries
}

bool Box2DSeparationRayShape2D::make_separation_ray(const Transform2D &p_transform, const Variant &p_data, b2Segment &p_segment) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::DICTIONARY, false);

	Dictionary dict = p_data;

	Rect2 rect;
	rect.position = Vector2();
	rect.size = Vector2(0, 1) * static_cast<float>(dict["length"]);

	p_segment.point1 = to_box2d(p_transform.xform(rect.position));
	p_segment.point2 = to_box2d(p_transform.xform(rect.size));

	return true;
}