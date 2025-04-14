#include "box2d_globals.h"

float BOX2D_PIXELS_PER_METER = 1;

ShapeCollideResult box2d_collide_shapes(
		const Box2DShapeGeometry &p_shape_a,
		const b2Transform &xfa,
		const Box2DShapeGeometry &p_shape_b,
		const b2Transform &xfb,
		bool p_swapped) {
	b2ShapeType type_a = p_shape_a.type;
	b2ShapeType type_b = p_shape_a.type;

	b2Manifold manifold;

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
					return {};
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
					return {};
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
					return {};
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
				case b2ShapeType::b2_segmentShape: {
					b2Segment b = p_shape_b.segment;
					b2Capsule capsule_b;
					capsule_b.radius = 0.0;
					capsule_b.center1 = b.point1;
					capsule_b.center2 = b.point2;
					b2CollideSegmentAndCapsule(&p_shape_a.segment, xfa, &capsule_b, xfb);
				}
				case b2ShapeType::b2_chainSegmentShape:
				default: {
					return {};
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
				case b2ShapeType::b2_chainSegmentShape:
				default: {
					return {};
				}
			}
			break;
		}
		default: {
			return {};
		}
	}

	ShapeCollideResult result;

	result.point_count = manifold.pointCount;

	if (result.point_count == 0) {
		return result;
	}

	result.normal = to_godot(manifold.normal).normalized();

	if (p_swapped) {
		result.normal *= -1;
	}

	for (int i = 0; i < manifold.pointCount; i++) {
		result.points[i].depth = -to_godot(manifold.points[i].separation);
		result.points[i].point = to_godot(manifold.points[i].point);
	}

	return result;
}

b2ShapeProxy box2d_make_shape_proxy(const Box2DShapeGeometry &p_shape) {
	switch (p_shape.type) {
		case b2_capsuleShape:
			return b2MakeProxy(&p_shape.capsule.center1, 2, p_shape.capsule.radius);
		case b2_circleShape:
			return b2MakeProxy(&p_shape.circle.center, 1, p_shape.circle.radius);
		case b2_polygonShape:
			return b2MakeProxy(p_shape.polygon.vertices, p_shape.polygon.count, p_shape.polygon.radius);
		case b2_segmentShape:
			return b2MakeProxy(&p_shape.segment.point1, 2, 0.0f);
		case b2_chainSegmentShape:
			return b2MakeProxy(&p_shape.chain_segment.segment.point1, 2, 0.0f);
		default: {
			B2_ASSERT(false);
			b2ShapeProxy empty = { 0 };
			return empty;
		}
	}
}
