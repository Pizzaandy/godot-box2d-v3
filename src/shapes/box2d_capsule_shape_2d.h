#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DCapsuleShape2D : public Box2DShape2D {
public:
	void add_to_body(Box2DShapeInstance *p_instance) const override;

	int cast(const CastQuery &p_query, const Transform2D &p_transform, LocalVector<CastHit> &p_results) const override;
	int overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const override;

	PhysicsServer2D::ShapeType get_type() const override { return PhysicsServer2D::ShapeType::SHAPE_CAPSULE; }

	static bool make_capsule(const Transform2D &p_transform, const Variant &p_data, b2Capsule &p_capsule);
};