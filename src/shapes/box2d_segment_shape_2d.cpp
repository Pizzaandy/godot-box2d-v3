#include "box2d_segment_shape_2d.h"

ShapeID Box2DSegmentShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::RECT2, b2_nullShapeId);

	Rect2 rect = data;

	b2Segment segment;
	segment.point1 = to_box2d(p_transform.xform(rect.position));
	segment.point2 = to_box2d(p_transform.xform(rect.size));

	return b2CreateSegmentShape(p_body, &p_shape_def, &segment);
}