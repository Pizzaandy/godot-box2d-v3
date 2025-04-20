#include "box2d_globals.h"

float BOX2D_PIXELS_PER_METER = 1;
float BOX2D_LINEAR_SLOP = 0.005f;

// TODO: revisit, consider implementing Godot-style cast function
float box2d_compute_safe_fraction(float p_unsafe_fraction, float p_total_distance) {
	if (p_total_distance <= 0.0f) {
		return 0.0f;
	}

	float distance = p_unsafe_fraction * p_total_distance;
	const float adjustment = 1.5f * to_godot(BOX2D_LINEAR_SLOP);
	float adjusted_distance = Math::max(0.0f, distance - adjustment);

	return adjusted_distance / p_total_distance;
}

ShapeCollideResult box2d_collide_shapes(
		const Box2DShapePrimitive &p_shape_a,
		const b2Transform &xfa,
		const Box2DShapePrimitive &p_shape_b,
		const b2Transform &xfb,
		bool p_swapped) {
	b2ShapeType type_a = p_shape_a.type;
	b2ShapeType type_b = p_shape_b.type;

	b2Manifold manifold = { 0 };

	switch (type_a) {
		case b2ShapeType::b2_capsuleShape: {
			b2Capsule a = p_shape_a.capsule;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape: {
					manifold = b2CollideCapsules(&a, xfa, &p_shape_b.capsule, xfb);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideCapsuleAndCircle(&a, xfa, &p_shape_b.circle, xfb);
					break;
				}
				case b2ShapeType::b2_polygonShape:
				case b2ShapeType::b2_segmentShape:
				case b2ShapeType::b2_chainSegmentShape: {
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				}
				default: {
					ERR_FAIL_V({});
				}
			}
			break;
		}
		case b2ShapeType::b2_circleShape: {
			b2Circle a = p_shape_a.circle;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape:
				case b2ShapeType::b2_polygonShape:
				case b2ShapeType::b2_segmentShape:
				case b2ShapeType::b2_chainSegmentShape: {
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideCircles(&a, xfa, &p_shape_b.circle, xfb);
					break;
				}
				default: {
					ERR_FAIL_V({});
				}
			}
			break;
		}
		case b2ShapeType::b2_polygonShape: {
			b2Polygon a = p_shape_a.polygon;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape: {
					manifold = b2CollidePolygonAndCapsule(&a, xfa, &p_shape_b.capsule, xfb);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollidePolygonAndCircle(&a, xfa, &p_shape_b.circle, xfb);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					manifold = b2CollidePolygons(&a, xfa, &p_shape_b.polygon, xfb);
					break;
				}
				case b2ShapeType::b2_segmentShape:
				case b2ShapeType::b2_chainSegmentShape: {
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				}
				default: {
					ERR_FAIL_V({});
				}
			}
			break;
		}
		case b2ShapeType::b2_segmentShape: {
			b2Segment a = p_shape_a.segment;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape: {
					manifold = b2CollideSegmentAndCapsule(&a, xfa, &p_shape_b.capsule, xfb);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideSegmentAndCircle(&a, xfa, &p_shape_b.circle, xfb);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					manifold = b2CollideSegmentAndPolygon(&a, xfa, &p_shape_b.polygon, xfb);
					break;
				}
				case b2ShapeType::b2_segmentShape:
				case b2ShapeType::b2_chainSegmentShape: {
					return {};
				}
				default: {
					ERR_FAIL_V({});
				}
			}
			break;
		}
		case b2ShapeType::b2_chainSegmentShape: {
			b2ChainSegment a = p_shape_a.chain_segment;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape: {
					b2SimplexCache cache{ 0 };
					manifold = b2CollideChainSegmentAndCapsule(&a, xfa, &p_shape_b.capsule, xfb, &cache);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideChainSegmentAndCircle(&a, xfa, &p_shape_b.circle, xfb);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					b2SimplexCache cache{ 0 };
					manifold = b2CollideChainSegmentAndPolygon(&a, xfa, &p_shape_b.polygon, xfb, &cache);
					break;
				}
				case b2ShapeType::b2_segmentShape:
				case b2ShapeType::b2_chainSegmentShape: {
					return {};
				}
				default: {
					ERR_FAIL_V({});
				}
			}
			break;
		}
		default: {
			ERR_FAIL_V({});
		}
	}

	ShapeCollideResult result;

	result.point_count = manifold.pointCount;

	if (result.point_count == 0) {
		return result;
	}

	result.normal = -to_godot_normalized(manifold.normal);

	if (p_swapped) {
		result.normal *= -1.0f;
	}

	for (int i = 0; i < manifold.pointCount; i++) {
		result.points[i].depth = -to_godot(manifold.points[i].separation);
		result.points[i].point = to_godot(manifold.points[i].point);
	}

	return result;
}