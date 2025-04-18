#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_2d.h"

class Box2DSeparationRayShape2D : public Box2DShape2D {
public:
	void add_to_body(Box2DShapeInstance *p_instance) const override;

	int cast(const CastQuery &p_query, LocalVector<CastHit> &p_results) const override { return 0; }
	int overlap(const OverlapQuery &p_query, LocalVector<ShapeOverlap> &p_results) const override { return 0; }

	PhysicsServer2D::ShapeType get_type() const override { return PhysicsServer2D::ShapeType::SHAPE_SEGMENT; }

	static bool make_separation_ray(const Transform2D &p_transform, const Variant &p_data, b2Segment &p_segment);
};