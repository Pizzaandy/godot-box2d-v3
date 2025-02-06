
#include "box2d_concave_polygon_shape_2d.h"

void Box2DConcavePolygonShape2D::add_to_body(b2BodyId p_body_id, Box2DShapeInstance *p_instance) const {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND(type != Variant::PACKED_VECTOR2_ARRAY);

	PackedVector2Array arr = data;

	int point_count = arr.size();
	ERR_FAIL_COND(point_count % 2);

	Transform2D shape_transform = p_instance->get_shape_transform();
	b2ShapeDef shape_def = p_instance->shape_def;

	for (int i = 0; i < point_count - 1; i++) {
		b2Vec2 point_a = to_box2d(shape_transform.xform(arr[i]));
		b2Vec2 point_b = to_box2d(shape_transform.xform(arr[i + 1]));
		b2Segment segment;
		segment.point1 = point_a;
		segment.point2 = point_b;
		b2ShapeId id = b2CreateSegmentShape(p_body_id, &shape_def, &segment);
		p_instance->shape_ids.push_back(id);
	}
}

ShapeGeometry Box2DConcavePolygonShape2D::get_shape_geometry(const Transform2D &p_transform) const {
	ERR_FAIL_V_MSG(ShapeGeometry::invalid(), "Box2D: Overlap and shape cast queries are not supported for concave polygons.");
}