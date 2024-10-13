#include "box2d_convex_polygon_shape_2d.h"
#include "../type_conversions.h"
#include <godot_cpp/variant/utility_functions.hpp>

b2ShapeId Box2DConvexPolygonShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
#ifdef REAL_T_IS_DOUBLE
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY && type != Variant::PACKED_FLOAT64_ARRAY, b2_nullShapeId);
#else
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY && type != Variant::PACKED_FLOAT32_ARRAY, b2_nullShapeId);
#endif
	int point_count;
	b2Vec2 *points;

	if (type == Variant::PACKED_VECTOR2_ARRAY) {
		PackedVector2Array arr = data;
		ERR_FAIL_COND_V(arr.is_empty(), b2_nullShapeId);

		point_count = arr.size();
		points = new b2Vec2[point_count];
		for (int i = 0; i < point_count; i++) {
			points[i] = to_box2d(p_transform.xform(arr[i]));
		}
	} else {
#ifdef REAL_T_IS_DOUBLE
		PackedFloat64Array arr = data;
#else
		PackedFloat32Array arr = data;
#endif
		ERR_FAIL_COND_V(arr.is_empty(), b2_nullShapeId);
		ERR_FAIL_COND_V(arr.size() % 4 == 0, b2_nullShapeId);

		point_count = arr.size() / 4;
		points = new b2Vec2[point_count];
		for (int i = 0; i < point_count; i += 2) {
			points[i] = to_box2d(p_transform.xform(Vector2(arr[i], arr[i + 1])));
		}
	}

	ERR_FAIL_COND_V(point_count < 3, b2_nullShapeId);

	ERR_FAIL_COND_V_MSG(
			point_count > b2_maxPolygonVertices,
			b2_nullShapeId,
			"Box2D: Convex polygons cannot have more than " + UtilityFunctions::str(b2_maxPolygonVertices) + "vertices");

	b2Hull hull = b2ComputeHull(points, point_count);

	b2Polygon polygon = b2MakePolygon(&hull, 0.0);
	return b2CreatePolygonShape(p_body, &p_shape_def, &polygon);
}