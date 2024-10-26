#include "box2d_globals.h"

float BOX2D_PIXELS_PER_METER = 1;

ShapeCollideResult box2d_collide_shapes(
		const ShapeInfo &p_shape_a,
		const b2Transform &xfa,
		const ShapeInfo &p_shape_b,
		const b2Transform &xfb,
		bool p_swapped) {
	ShapeInfo::Type type_a = p_shape_a.type;
	ShapeInfo::Type type_b = p_shape_b.type;

	b2Manifold manifold;
	ShapeCollideResult result;

	switch (type_a) {
		case ShapeInfo::Type::CAPSULE:
			switch (type_b) {
				case ShapeInfo::Type::CAPSULE:
					manifold = b2CollideCapsules(&p_shape_a.capsule, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeInfo::Type::CIRCLE:
					manifold = b2CollideCapsuleAndCircle(&p_shape_a.capsule, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeInfo::Type::POLYGON:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeInfo::Type::CIRCLE:
			switch (type_b) {
				case ShapeInfo::Type::CAPSULE:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::CIRCLE:
					manifold = b2CollideCircles(&p_shape_a.circle, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeInfo::Type::POLYGON:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeInfo::Type::POLYGON:
			switch (type_b) {
				case ShapeInfo::Type::CAPSULE:
					manifold = b2CollidePolygonAndCapsule(&p_shape_a.polygon, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeInfo::Type::CIRCLE:
					manifold = b2CollidePolygonAndCircle(&p_shape_a.polygon, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeInfo::Type::POLYGON:
					manifold = b2CollidePolygons(&p_shape_a.polygon, xfa, &p_shape_b.polygon, xfb);
					break;
				case ShapeInfo::Type::SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				case ShapeInfo::Type::CHAIN_SEGMENT:
					return box2d_collide_shapes(p_shape_b, xfb, p_shape_a, xfa, true);
				default:
					return {};
			}
			break;
		case ShapeInfo::Type::SEGMENT:
			switch (type_b) {
				case ShapeInfo::Type::CAPSULE:
					manifold = b2CollideSegmentAndCapsule(&p_shape_a.segment, xfa, &p_shape_b.capsule, xfb);
					break;
				case ShapeInfo::Type::CIRCLE:
					manifold = b2CollideSegmentAndCircle(&p_shape_a.segment, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeInfo::Type::POLYGON:
					manifold = b2CollideSegmentAndPolygon(&p_shape_a.segment, xfa, &p_shape_b.polygon, xfb);
					break;
				case ShapeInfo::Type::SEGMENT:
					return {};
				case ShapeInfo::Type::CHAIN_SEGMENT:
					return {};
				default:
					return {};
			}
			break;
		case ShapeInfo::Type::CHAIN_SEGMENT:
			switch (type_b) {
				case ShapeInfo::Type::CAPSULE:
					manifold = b2CollideChainSegmentAndCapsule(&p_shape_a.chainSegment, xfa, &p_shape_b.capsule, xfb, { 0 });
					break;
				case ShapeInfo::Type::CIRCLE:
					manifold = b2CollideChainSegmentAndCircle(&p_shape_a.chainSegment, xfa, &p_shape_b.circle, xfb);
					break;
				case ShapeInfo::Type::POLYGON:
					manifold = b2CollideChainSegmentAndPolygon(&p_shape_a.chainSegment, xfa, &p_shape_b.polygon, xfb, { 0 });
					break;
				case ShapeInfo::Type::SEGMENT:
					return {};
				case ShapeInfo::Type::CHAIN_SEGMENT:
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
		result.points[i].point = to_godot(manifold.points[i].point) - (0.5 * result.points[i].separation * result.normal);
	}

	return result;
}

void box2d_cast_shape(
		const b2WorldId &p_world,
		const ShapeInfo &p_shape,
		const b2Transform &p_transform,
		const b2Vec2 &p_motion,
		const b2QueryFilter &p_filter,
		b2CastResultFcn *fcn,
		void *context) {
	b2Capsule capsule;

	switch (p_shape.type) {
		case ShapeInfo::Type::CAPSULE:
			b2World_CastCapsule(p_world, &p_shape.capsule, p_transform, p_motion, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::CIRCLE:
			b2World_CastCircle(p_world, &p_shape.circle, p_transform, p_motion, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::POLYGON:
			b2World_CastPolygon(p_world, &p_shape.polygon, p_transform, p_motion, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::SEGMENT:
			b2Capsule capsule;
			capsule.center1 = p_shape.segment.point1;
			capsule.center2 = p_shape.segment.point2;
			capsule.radius = 0.0;
			b2World_CastCapsule(p_world, &capsule, p_transform, p_motion, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::CHAIN_SEGMENT:
			capsule.center1 = p_shape.chainSegment.segment.point1;
			capsule.center2 = p_shape.chainSegment.segment.point2;
			capsule.radius = 0.0;
			b2World_CastCapsule(p_world, &capsule, p_transform, p_motion, p_filter, fcn, context);
			return;
		default:
			return;
	}
}

void box2d_overlap_shape(
		const b2WorldId &p_world,
		const ShapeInfo &p_shape,
		const b2Transform &p_transform,
		const b2QueryFilter &p_filter,
		b2OverlapResultFcn *fcn,
		void *context) {
	b2Capsule capsule;

	switch (p_shape.type) {
		case ShapeInfo::Type::CAPSULE:
			b2World_OverlapCapsule(p_world, &p_shape.capsule, p_transform, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::CIRCLE:
			b2World_OverlapCircle(p_world, &p_shape.circle, p_transform, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::POLYGON:
			b2World_OverlapPolygon(p_world, &p_shape.polygon, p_transform, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::SEGMENT:
			capsule.center1 = p_shape.segment.point1;
			capsule.center2 = p_shape.segment.point2;
			capsule.radius = 0.0;
			b2World_OverlapCapsule(p_world, &capsule, p_transform, p_filter, fcn, context);
			return;
		case ShapeInfo::Type::CHAIN_SEGMENT:
			capsule.center1 = p_shape.chainSegment.segment.point1;
			capsule.center2 = p_shape.chainSegment.segment.point2;
			capsule.radius = 0.0;
			b2World_OverlapCapsule(p_world, &capsule, p_transform, p_filter, fcn, context);
			return;
		default:
			return;
	}
}