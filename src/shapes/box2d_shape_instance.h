#pragma once

#include "box2d_shape_2d.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

class Box2DShape2D;
class Box2DCollisionObject2D;

class Box2DShapeInstance {
public:
	explicit Box2DShapeInstance(Box2DCollisionObject2D *p_object,
			Box2DShape2D *p_shape,
			const Transform2D &p_transform,
			bool p_disabled);

	Box2DShapeInstance() = default;

	~Box2DShapeInstance();

	void set_shape(Box2DShape2D *p_shape);
	Box2DShape2D *get_shape() const { return shape; }

	void set_index(int p_index) { index = p_index; }
	int get_index() const { return index; }

	void set_transform(const Transform2D &p_transform) { transform = p_transform; }
	Transform2D get_transform() const { return transform; }

	void set_disabled(bool p_disabled) { disabled = p_disabled; }
	bool get_disabled() const { return disabled; }

	void set_one_way_collision(bool p_one_way) { one_way_collision = p_one_way; }
	bool get_one_way_collision() const { return one_way_collision; }

	void set_one_way_collision_margin(float p_margin) { one_way_collision_margin = p_margin; }
	float get_one_way_collision_margin() const { return one_way_collision_margin; }

	bool should_filter_one_way_collision(const Vector2 &p_motion, const Vector2 &p_normal, float p_depth) const;

	Transform2D get_global_transform() const;
	Transform2D get_global_transform_with_parent_transform(const Transform2D &p_parent_transform) const;
	b2ShapeDef get_shape_def();

	void build();

	void add_shape_id(b2ShapeId p_id) { shape_ids.push_back(p_id); }
	const LocalVector<b2ShapeId> &get_shape_ids() { return shape_ids; }
	void clear_shape_ids() { shape_ids.clear(); }

	bool is_separation_ray() const { return shape_type == PhysicsServer2D::SHAPE_SEPARATION_RAY; }

	Box2DCollisionObject2D *get_collision_object() const { return object; }

private:
	Box2DCollisionObject2D *object = nullptr;
	Box2DShape2D *shape = nullptr;
	PhysicsServer2D::ShapeType shape_type = PhysicsServer2D::SHAPE_CUSTOM;

	LocalVector<b2ShapeId> shape_ids;

	int index = -1;
	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 0.0;
};