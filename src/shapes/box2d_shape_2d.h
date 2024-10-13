#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/classes/physics_server2d.hpp>

using namespace godot;

/// @brief Builder class for shapes. Does not keep track of shape instances.
class Box2DShape2D {
public:
	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }
	virtual b2ShapeId build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) = 0;
	void set_data(const Variant &p_data) { data = p_data; }
	Variant get_data() const { return data; }

protected:
	RID rid;
	Variant data;
};