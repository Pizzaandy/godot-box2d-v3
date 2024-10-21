#pragma once

#include "../box2d_globals.h"
#include "box2d_shape_instance.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/hash_set.hpp>

using namespace godot;

class Box2DShapeInstance;
struct ShapeID;

/// @brief Builder class for shapes.
class Box2DShape2D {
public:
	~Box2DShape2D();

	virtual ShapeID build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) = 0;
	virtual void cast_shape(b2WorldId p_world, Transform2D p_transform, Vector2 p_motion, b2QueryFilter p_filter, b2CastResultFcn *fcn, void *context) = 0;

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }
	void set_data(const Variant &p_data) { data = p_data; }
	Variant get_data() const { return data; }

	void add_instance(Box2DShapeInstance *p_shape) { instances.insert(p_shape); }
	void remove_instance(Box2DShapeInstance *p_shape) { instances.erase(p_shape); }

protected:
	RID rid;
	Variant data;
	HashSet<Box2DShapeInstance *> instances;
};