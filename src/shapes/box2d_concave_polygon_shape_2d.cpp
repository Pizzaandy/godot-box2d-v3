#include "box2d_concave_polygon_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DConcavePolygonShape2D::add_to_body(Box2DShapeInstance *p_instance) const {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND(type != Variant::PACKED_VECTOR2_ARRAY);
	PackedVector2Array arr = data;
	ERR_FAIL_COND(arr.size() % 2);

	Transform2D shape_transform = p_instance->get_global_transform();
	b2ShapeDef shape_def = p_instance->get_shape_def();

	for (int i = 0; i < arr.size() - 1; i++) {
		b2Vec2 point_a = to_box2d(shape_transform.xform(arr[i]));
		b2Vec2 point_b = to_box2d(shape_transform.xform(arr[i + 1]));
		b2Segment segment;
		segment.point1 = point_a;
		segment.point2 = point_b;
		b2ShapeId id = b2CreateSegmentShape(p_instance->get_collision_object()->get_body_id(), &shape_def, &segment);
		p_instance->add_shape_id(id);
	}
}

// int Box2DConcavePolygonShape2D::cast(const CastQuery &p_query, LocalVector<CastHit> &p_results) const {
// 	Variant::Type type = data.get_type();
// 	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY, 0);
// 	PackedVector2Array arr = data;
// 	ERR_FAIL_COND_V(arr.size() % 2, 0);

// 	CastQuery query = p_query;
// 	LocalVector<CastHit> results;

// 	Transform2D shape_transform = p_query.transform;
// 	b2Capsule capsule;
// 	capsule.radius = 0.0;

// 	for (int i = 0; i < arr.size() - 1; i++) {
// 		capsule.center1 = to_box2d(shape_transform.xform(arr[i]));
// 		capsule.center2 = to_box2d(shape_transform.xform(arr[i + 1]));

// 		CastQueryCollector collector(p_query, results);
// 		b2ShapeProxy proxy = box2d_make_shape_proxy(capsule);
// 		b2World_CastShape(p_query.world, &proxy, to_box2d(p_query.translation), p_query.filter.filter, cast_callback, &collector);
// 		query.max_results -= collector.count;
// 		if (query.max_results <= 0) {
// 			break;
// 		}
// 		if (collector.count == 0) {
// 			continue;
// 		}
// 		if (p_query.find_nearest) {
// 			CastHit last_hit = results[results.size() - 1];
// 			if (last_hit.fraction <= 0.0) {
// 				break;
// 			}
// 			query.translation *= last_hit.fraction;
// 		}
// 	}

// 	// For each shape hit, keep the nearest CastHit only
// 	int i = 0;
// 	while (i < results.size()) {
// 		int j = i + 1;
// 		while (j < results.size()) {
// 			if (results[j] == results[i]) {
// 				results.remove_at(j);
// 			} else {
// 				++j;
// 			}
// 		}
// 		++i;
// 	}

// 	for (CastHit &hit : results) {
// 		p_results.push_back(hit);
// 	}

// 	return results.size();
// }

int Box2DConcavePolygonShape2D::overlap(const OverlapQuery &p_query, LocalVector<ShapeOverlap> &p_results) const {
	Variant::Type type = data.get_type();
	ERR_FAIL_COND_V(type != Variant::PACKED_VECTOR2_ARRAY, 0);
	PackedVector2Array arr = data;
	ERR_FAIL_COND_V(arr.size() % 2, 0);

	OverlapQuery query = p_query;
	LocalVector<ShapeOverlap> results;

	Transform2D shape_transform = p_query.transform;
	b2Capsule capsule;
	capsule.radius = 0.0;

	for (int i = 0; i < arr.size() - 1; i++) {
		capsule.center1 = to_box2d(shape_transform.xform(arr[i]));
		capsule.center2 = to_box2d(shape_transform.xform(arr[i + 1]));

		OverlapQueryCollector collector(query, results);
		b2ShapeProxy proxy = box2d_make_shape_proxy(capsule);
		b2World_OverlapShape(p_query.world, &proxy, p_query.filter.filter, overlap_callback, &collector);
		query.max_results -= collector.count;
		if (query.max_results <= 0) {
			break;
		}
	}

	// De-duplicate overlaps
	int i = 0;
	while (i < results.size()) {
		int j = i + 1;
		while (j < results.size()) {
			if (results[j] == results[i]) {
				results.remove_at(j);
			} else {
				++j;
			}
		}
		++i;
	}

	for (ShapeOverlap &overlap : results) {
		p_results.push_back(overlap);
	}

	return results.size();
}