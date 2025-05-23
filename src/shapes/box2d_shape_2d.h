#pragma once

#include "../box2d_globals.h"
#include "../spaces/box2d_query.h"
#include "box2d_shape_instance.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>

using namespace godot;

class Box2DShapeInstance;
class Box2DCollisionObject2D;

class Box2DShape2D {
public:
	virtual ~Box2DShape2D();

	/// Note: This does not validate data
	virtual void add_to_body(Box2DShapeInstance *p_instance) const = 0;
	virtual void remove_from_body(Box2DShapeInstance *p_instance) const;

	virtual int cast(const CastQuery &p_query, const Transform2D &p_transform, LocalVector<CastHit> &p_results) const = 0;
	virtual int overlap(const OverlapQuery &p_query, const Transform2D &p_transform, LocalVector<ShapeOverlap> &p_results) const = 0;

	virtual PhysicsServer2D::ShapeType get_type() const = 0;

	void set_rid(const RID &p_rid) { rid = p_rid; }
	RID get_rid() const { return rid; }

	void set_data(const Variant &p_data);
	Variant get_data() const { return data; }

	void add_object_reference(Box2DCollisionObject2D *p_owner);
	void remove_object_reference(Box2DCollisionObject2D *p_owner);
	void remove_from_all_objects();

protected:
	void update_instances();

	RID rid;
	Variant data;
	HashMap<Box2DCollisionObject2D *, int> ref_counts_by_object;
};