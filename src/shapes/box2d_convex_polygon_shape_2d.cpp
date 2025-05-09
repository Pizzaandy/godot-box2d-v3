#include "box2d_convex_polygon_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"
#include <godot_cpp/variant/utility_functions.hpp>

void Box2DConvexPolygonShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	b2Polygon shape;
	if (!make_polygon(p_instance->get_global_transform(), data, shape)) {
		return;
	}
	b2ShapeDef shape_def = p_instance->get_shape_def();
	b2ShapeId id = b2CreatePolygonShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &shape);
	p_instance->add_shape_id(id);
}

int Box2DConvexPolygonShape2D::cast(const CastQuery &p_query, const Transform2D &p_transform, LocalVector<CastHit> &p_results) const {
	b2Polygon shape;
	if (!make_polygon(p_transform, data, shape)) {
		return 0;
	}
	return box2d_cast_shape(shape, p_query, p_results);
}

int Box2DConvexPolygonShape2D::overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const {
	b2Polygon shape;
	if (!make_polygon(p_transform, data, shape)) {
		return 0;
	}
	return box2d_overlap_shape(shape, p_query, p_results);
}

bool Box2DConvexPolygonShape2D::make_polygon(const Transform2D &p_transform, const Variant &p_data, b2Polygon &p_polygon) {
	Variant::Type type = p_data.get_type();
#ifdef REAL_T_IS_DOUBLE
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY && type != Variant::PACKED_FLOAT64_ARRAY, false);
#else
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY && type != Variant::PACKED_FLOAT32_ARRAY, false);
#endif

	int point_count;
	b2Vec2 *points = nullptr;

	if (type == Variant::PACKED_VECTOR2_ARRAY) {
		PackedVector2Array arr = p_data;
		ERR_FAIL_COND_V(arr.is_empty(), false);

		point_count = arr.size();
		points = memnew_arr(b2Vec2, point_count);
		for (int i = 0; i < point_count; i++) {
			points[i] = to_box2d(p_transform.xform(arr[i]));
		}
	} else {
#ifdef REAL_T_IS_DOUBLE
		PackedFloat64Array arr = p_data;
#else
		PackedFloat32Array arr = p_data;
#endif
		ERR_FAIL_COND_V(arr.is_empty(), false);
		ERR_FAIL_COND_V(arr.size() % 4 == 0, false);

		point_count = arr.size() / 4;
		points = memnew_arr(b2Vec2, point_count);
		for (int i = 0; i < point_count; i += 2) {
			points[i] = to_box2d(p_transform.xform(Vector2(arr[i], arr[i + 1])));
		}
	}

	ERR_FAIL_COND_V(point_count < 3, false);

	ERR_FAIL_COND_V_MSG(
			point_count > B2_MAX_POLYGON_VERTICES,
			false,
			"Box2D: Convex polygons cannot have more than " + itos(B2_MAX_POLYGON_VERTICES) + " vertices");

	b2Hull hull = b2ComputeHull(points, point_count);

	memdelete_arr(points);

	ERR_FAIL_COND_V_MSG(hull.count == 0, false, "Box2D: Failed to compute convex hull for polygon");

	p_polygon = b2MakePolygon(&hull, 0.0);

	return true;
}