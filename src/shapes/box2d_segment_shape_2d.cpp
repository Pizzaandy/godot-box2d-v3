#include "box2d_segment_shape_2d.h"

ShapeID Box2DSegmentShape2D::build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	b2Segment segment;

	if (!make_segment(p_transform, data, segment)) {
		return ShapeID::invalid();
	}

	return b2CreateSegmentShape(p_body, &p_shape_def, &segment);
}

ShapeInfo Box2DSegmentShape2D::get_shape_info(const Transform2D &p_transform) const {
	ShapeInfo shape;
	shape.type = ShapeInfo::Type::SEGMENT;
	if (!make_segment(p_transform, data, shape.segment)) {
		return ShapeInfo::invalid();
	}
	return shape;
}

bool Box2DSegmentShape2D::make_segment(const Transform2D &p_transform, const Variant &p_data, b2Segment &p_segment) {
	Variant::Type type = p_data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, false);

	Rect2 rect = p_data;

	p_segment.point1 = to_box2d(p_transform.xform(rect.position));
	p_segment.point2 = to_box2d(p_transform.xform(rect.size));

	return true;
}