#include "box2d_segment_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DSegmentShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Segment shape;
	if (!make_segment(p_instance->get_global_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreateSegmentShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

int Box2DSegmentShape2D::overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const {
	b2Capsule shape;
	if (!make_capsule_segment(p_transform, data, shape)) {
		return 0;
	}
	return box2d_overlap_shape(shape, p_query, p_results);
}

bool Box2DSegmentShape2D::make_segment(const Transform2D &p_transform, const Variant &p_data, b2Segment &p_segment) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, false);

	Rect2 rect = p_data;

	p_segment.point1 = to_box2d(p_transform.xform(rect.position));
	p_segment.point2 = to_box2d(p_transform.xform(rect.size));

	return true;
}

bool Box2DSegmentShape2D::make_capsule_segment(const Transform2D &p_transform, const Variant &p_data, b2Capsule &p_capsule) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, false);

	Rect2 rect = p_data;

	p_capsule.radius = 0.0;
	p_capsule.center1 = to_box2d(p_transform.xform(rect.position));
	p_capsule.center2 = to_box2d(p_transform.xform(rect.size));

	return true;
}