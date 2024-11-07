#include "box2d_segment_shape_2d.h"

ShapeIdAndGeometry Box2DSegmentShape2D::add_to_body(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) const {
	ShapeIdAndGeometry result;

	result.info = get_shape_geometry(p_transform);
	if (!result.info.is_valid()) {
		return result;
	}

	result.id = b2CreateSegmentShape(p_body, &p_shape_def, &result.info.segment);
	return result;
}

ShapeGeometry Box2DSegmentShape2D::get_shape_geometry(const Transform2D &p_transform) const {
	ShapeGeometry shape;
	shape.type = ShapeGeometry::SEGMENT;
	if (!make_segment(p_transform, data, shape.segment)) {
		return ShapeGeometry::invalid();
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