#include "box2d_globals.h"

float BOX2D_PIXELS_PER_METER = 1;
float BOX2D_LINEAR_SLOP = 0.005f;

// TODO: revisit, consider implementing Godot-style cast function
float box2d_compute_safe_fraction(float p_unsafe_fraction, float p_total_distance, float p_amount) {
	if (p_amount <= 0.0f) {
		p_amount = to_godot(BOX2D_LINEAR_SLOP) * 1.5f;
	}

	if (p_total_distance <= 0.0f) {
		return 0.0f;
	}

	if (p_unsafe_fraction >= 1.0f) {
		return 1.0f;
	}

	float distance = p_unsafe_fraction * p_total_distance;
	float adjusted_distance = Math::clamp(distance - p_amount, 0.0f, p_total_distance);
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

	b2LocalManifold manifold = { 0 };
	b2Transform xform = b2InvMulWorldTransforms(xfa, xfb);

	switch (type_a) {
		case b2ShapeType::b2_capsuleShape: {
			b2Capsule a = p_shape_a.capsule;
			switch (type_b) {
				case b2ShapeType::b2_capsuleShape: {
					manifold = b2CollideCapsules(&a, &p_shape_b.capsule, xform);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideCapsuleAndCircle(&a, &p_shape_b.circle, xform);
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
					manifold = b2CollideCircles(&a, &p_shape_b.circle, xform);
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
					manifold = b2CollidePolygonAndCapsule(&a, &p_shape_b.capsule, xform);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollidePolygonAndCircle(&a, &p_shape_b.circle, xform);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					manifold = b2CollidePolygons(&a, &p_shape_b.polygon, xform);
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
					manifold = b2CollideSegmentAndCapsule(&a, &p_shape_b.capsule, xform);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideSegmentAndCircle(&a, &p_shape_b.circle, xform);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					manifold = b2CollideSegmentAndPolygon(&a, &p_shape_b.polygon, xform);
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
					manifold = b2CollideChainSegmentAndCapsule(&a, &p_shape_b.capsule, xform, &cache);
					break;
				}
				case b2ShapeType::b2_circleShape: {
					manifold = b2CollideChainSegmentAndCircle(&a, &p_shape_b.circle, xform);
					break;
				}
				case b2ShapeType::b2_polygonShape: {
					b2SimplexCache cache{ 0 };
					manifold = b2CollideChainSegmentAndPolygon(&a, &p_shape_b.polygon, xform, &cache);
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

	b2WorldTransform xform_a = b2MakeWorldTransform(xfa);
	b2Vec2 world_normal = b2InvRotateVector(xform_a.q, manifold.normal);
	result.normal = -to_godot_normalized(world_normal);
	if (p_swapped) {
		result.normal *= -1.0f;
	}

	for (int i = 0; i < manifold.pointCount; i++) {
		result.points[i].depth = -to_godot(manifold.points[i].separation);
		b2Pos world_point_a = b2TransformWorldPoint(xform_a, manifold.points[i].point);
		result.points[i].point = to_godot(world_point_a) + (0.5f * result.points[i].depth * result.normal);
	}

	return result;
}
