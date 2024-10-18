
#include "box2d_concave_polygon_shape_2d.h"
#include "../type_conversions.h"
#include <godot_cpp/variant/utility_functions.hpp>

Box2DShape2D::ShapeID Box2DConcavePolygonShape2D::build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY, {});

	PackedVector2Array arr = data;
	int point_count = arr.size();
	ERR_FAIL_COND_V_MSG(point_count < 4, {}, "Box2D: Concave polygons must have at least 4 vertices");

	b2Vec2 *points = new b2Vec2[point_count];
	for (int i = 0; i < point_count; i++) {
		points[i] = to_box2d(p_transform.xform(arr[i]));
	}

	b2ChainDef chain_def = b2DefaultChainDef();
	chain_def.points = points;
	chain_def.count = point_count;
	chain_def.isLoop = true;
	chain_def.filter = p_shape_def.filter;

	Box2DShape2D::ShapeID id = b2CreateChain(p_body, &chain_def);

	delete[] points;

	return id;
}