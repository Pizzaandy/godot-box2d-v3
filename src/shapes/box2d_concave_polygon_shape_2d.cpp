
#include "box2d_concave_polygon_shape_2d.h"

ShapeID Box2DConcavePolygonShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY, {});

	PackedVector2Array arr = data;

	int point_count = arr.size();
	ERR_FAIL_COND_V_MSG(point_count < 4, {}, "Box2D: Concave polygons must have at least 4 vertices");

	if (is_polygon_clockwise(arr)) {
		arr.reverse();
	}

	b2Vec2 *points = new b2Vec2[point_count];

	for (int i = 0; i < point_count; i++) {
		points[i] = to_box2d(p_transform.xform(arr[i]));
	}

	b2ChainDef chain_def = b2DefaultChainDef();
	chain_def.points = points;
	chain_def.count = point_count;
	chain_def.isLoop = true;
	chain_def.filter = p_shape_def.filter;

	ShapeID id = b2CreateChain(p_body, &chain_def);

	delete[] points;

	return id;
}

void Box2DConcavePolygonShape2D::cast_shape(
		b2WorldId p_world,
		Transform2D p_transform,
		Vector2 p_motion,
		b2QueryFilter p_filter,
		b2CastResultFcn *fcn,
		void *context) {
	ERR_PRINT_ONCE("Box2D: Shape casting concave polygons is not supported");
}

bool Box2DConcavePolygonShape2D::is_polygon_clockwise(const PackedVector2Array &p_polygon) {
	int c = p_polygon.size();

	if (c < 3) {
		return false;
	}

	const Vector2 *p = p_polygon.ptr();
	real_t sum = 0;
	for (int i = 0; i < c; i++) {
		const Vector2 &v1 = p[i];
		const Vector2 &v2 = p[(i + 1) % c];
		sum += (v2.x - v1.x) * (v2.y + v1.y);
	}

	return sum > 0.0f;
}