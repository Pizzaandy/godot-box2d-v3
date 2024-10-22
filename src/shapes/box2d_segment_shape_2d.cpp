#include "box2d_segment_shape_2d.h"

ShapeID Box2DSegmentShape2D::build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	b2Segment segment;

	if (!make_segment(p_transform, data, segment)) {
		return {};
	}

	return b2CreateSegmentShape(p_body, &p_shape_def, &segment);
}

void Box2DSegmentShape2D::cast_shape(
		b2WorldId p_world,
		const Transform2D &p_transform,
		Vector2 p_motion,
		b2QueryFilter p_filter,
		b2CastResultFcn *fcn,
		void *context) const {
	b2Capsule fat_segment;

	if (!make_fat_segment(p_transform, data, to_box2d(0.5), fat_segment)) {
		return;
	}

	b2World_CastCapsule(p_world, &fat_segment, b2Transform_identity, to_box2d(p_motion), p_filter, fcn, context);
}

bool Box2DSegmentShape2D::make_segment(const Transform2D &p_transform, const Variant &p_data, b2Segment &p_segment) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, false);

	Rect2 rect = p_data;

	p_segment.point1 = to_box2d(p_transform.xform(rect.position));
	p_segment.point2 = to_box2d(p_transform.xform(rect.size));

	return true;
}

bool Box2DSegmentShape2D::make_fat_segment(const Transform2D &p_transform, const Variant &p_data, float radius, b2Capsule &p_capsule) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, false);

	Rect2 rect = p_data;

	p_capsule.center1 = to_box2d(p_transform.xform(rect.position));
	p_capsule.center2 = to_box2d(p_transform.xform(rect.size));
	p_capsule.radius = radius;

	return true;
}