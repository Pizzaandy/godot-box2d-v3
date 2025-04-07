#include "box2d_globals.h"

float BOX2D_PIXELS_PER_METER = 1;

ShapeCollideResult box2d_collide_shapes(
		const ShapeGeometry &p_shape_a,
		const b2Transform &xfa,
		const ShapeGeometry &p_shape_b,
		const b2Transform &xfb,
		bool p_swapped) {
	ShapeGeometry::Type type_a = p_shape_a.type;
	ShapeGeometry::Type type_b = p_shape_b.type;

	b2Manifold manifold;
	ShapeCollideResult result;

	switch (type_a) {
		case ShapeGeometry::Type::CAPSULE:
			switch (type_b) {
				case ShapeGeometry::Type::CAPSULE:
					manifold = b2CollideCapsules(&p_shape_a.capsule, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeGeometry::Type::CIRCLE:
					manifold = b2CollideCapsuleAndCircle(&p_shape_a.capsule, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeGeometry::Type::POLYGON:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeGeometry::Type::CIRCLE:
			switch (type_b) {
				case ShapeGeometry::Type::CAPSULE:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::CIRCLE:
					manifold = b2CollideCircles(&p_shape_a.circle, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeGeometry::Type::POLYGON:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeGeometry::Type::POLYGON:
			switch (type_b) {
				case ShapeGeometry::Type::CAPSULE:
					manifold = b2CollidePolygonAndCapsule(&p_shape_a.polygon, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeGeometry::Type::CIRCLE:
					manifold = b2CollidePolygonAndCircle(&p_shape_a.polygon, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeGeometry::Type::POLYGON:
					manifold = b2CollidePolygons(&p_shape_a.polygon, xfa, &p_shape_b.polygon, xfb);
					break;
				case ShapeGeometry::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeGeometry::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeGeometry::Type::SEGMENT:
			switch (type_b) {
				case ShapeGeometry::Type::CAPSULE:
					manifold = b2CollideSegmentAndCapsule(&p_shape_a.segment, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeGeometry::Type::CIRCLE:
					manifold = b2CollideSegmentAndCircle(&p_shape_a.segment, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeGeometry::Type::POLYGON:
					manifold = b2CollideSegmentAndPolygon(&p_shape_a.segment, xfa, &p_shape_b.polygon, xfb);
					break;
				case ShapeGeometry::Type::SEGMENT:
					return {};
				case ShapeGeometry::Type::CHAIN_SEGMENT:
					return {};
				default:
					return {};
			}
			break;
		case ShapeGeometry::Type::CHAIN_SEGMENT:
			switch (type_b) {
				case ShapeGeometry::Type::CAPSULE: {
					b2SimplexCache cache{ 0 };
					manifold = b2CollideChainSegmentAndCapsule(&p_shape_a.chain_segment, xfa, &p_shape_b.capsule, xfb, &cache);
					break;
				}
				case ShapeGeometry::Type::CIRCLE:
					manifold = b2CollideChainSegmentAndCircle(&p_shape_a.chain_segment, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeGeometry::Type::POLYGON: {
					b2SimplexCache cache{ 0 };
					manifold = b2CollideChainSegmentAndPolygon(&p_shape_a.chain_segment, xfa, &p_shape_b.polygon, xfb, &cache);
					break;
				}
				case ShapeGeometry::Type::SEGMENT:
					return {};
				case ShapeGeometry::Type::CHAIN_SEGMENT:
					return {};
				default:
					return {};
			}
			break;
		default:
			return {};
	}

	result.point_count = manifold.pointCount;

	if (result.point_count == 0) {
		return result;
	}

	result.normal = -to_godot(manifold.normal).normalized();

	if (p_swapped) {
		result.normal *= -1;
	}

	for (int i = 0; i < manifold.pointCount; i++) {
		result.points[i].separation = to_godot(manifold.points[i].separation);
		// TODO: change when fixed upstream
		result.points[i].point = to_godot(manifold.points[i].point) - (0.5 * result.points[i].separation * result.normal);
	}

	return result;
}