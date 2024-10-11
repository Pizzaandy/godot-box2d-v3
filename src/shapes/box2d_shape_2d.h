#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/classes/physics_server2d.hpp>

using namespace godot;

/// @brief Builder class for shapes. Does not keep track of shape instances.
class Box2DShape2D {
public:
	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }
	virtual b2ShapeId build(b2BodyId p_body_id, Transform2D p_transform, bool p_disabled, b2ShapeId p_shape_id = b2_nullShapeId) = 0;
	void set_data(const Variant &p_data) { data = p_data; }
	Variant get_data() const { return data; }

protected:
	RID rid;
	Variant data;
};