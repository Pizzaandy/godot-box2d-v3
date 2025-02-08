#pragma once

#include "../box2d_globals.h"
#include "../box2d_project_settings.h"
#include "../shapes/box2d_shape_2d.h"
#include "../shapes/box2d_shape_instance.h"
#include "../spaces/box2d_query_collectors.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

class Box2DSpace2D;

class Box2DCollisionObject2D {
public:
	enum Type {
		RIGIDBODY,
		AREA,
	};

	explicit Box2DCollisionObject2D(Type p_type);

	Type get_type() const { return type; }
	bool is_area() const { return type == Type::AREA; }

	void destroy_body();

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }

	void set_space(Box2DSpace2D *p_space);
	Box2DSpace2D *get_space() const { return space; }

	void set_mode(PhysicsServer2D::BodyMode p_mode);
	PhysicsServer2D::BodyMode get_mode() const { return mode; }

	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() { return shape_def.filter.categoryBits; }
	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() { return shape_def.filter.maskBits; }

	void set_transform(const Transform2D &p_transform, bool p_move_kinematic = false);
	Transform2D get_transform() const { return current_transform; }

	b2BodyId get_body_id() const { return body_id; }
	b2ShapeDef get_shape_def() const { return shape_def; }

	void add_shape(Box2DShape2D *p_shape, const Transform2D &p_transform, bool p_disabled);
	void set_shape(int p_index, Box2DShape2D *p_shape);
	void remove_shape(int p_index);
	void remove_shape(Box2DShape2D *p_shape);
	void clear_shapes();
	void reindex_all_shapes();

	int32_t get_shape_count() const { return shapes.size(); }
	void set_shape_transform(int p_index, const Transform2D &p_transform);
	Transform2D get_shape_transform(int p_index) const;
	RID get_shape_rid(int p_index) const;
	void set_shape_disabled(int p_index, bool p_disabled);

	void set_instance_id(const ObjectID &p_instance_id) { instance_id = p_instance_id; }
	ObjectID get_instance_id() const { return instance_id; }

	void set_canvas_instance_id(const ObjectID &p_canvas_instance_id) { canvas_instance_id = p_canvas_instance_id; }
	ObjectID get_canvas_instance_id() const { return canvas_instance_id; }

	void set_user_data(const Variant &p_data) { user_data = p_data; }
	Variant get_user_data() const { return user_data; }

	virtual uint64_t modify_mask_bits(uint32_t p_mask) { return p_mask; }
	virtual uint64_t modify_layer_bits(uint32_t p_layer) { return p_layer; }
	virtual void shapes_changed() {};
	virtual void on_body_created() {};
	virtual void on_destroy_body() {};

protected:
	void build_shape(Box2DShapeInstance &p_shape, bool p_shapes_changed = true);
	void rebuild_all_shapes();

	Variant user_data = Variant();

	Box2DSpace2D *space = nullptr;
	RID rid;
	ObjectID instance_id;
	ObjectID canvas_instance_id;
	Transform2D current_transform;
	LocalVector<Box2DShapeInstance> shapes;
	PhysicsServer2D::BodyMode mode = PhysicsServer2D::BodyMode::BODY_MODE_STATIC;

	b2BodyDef body_def = b2DefaultBodyDef();
	b2ShapeDef shape_def = b2DefaultShapeDef();
	b2BodyId body_id = b2_nullBodyId;

	bool body_exists = false;
	Type type = Type::RIGIDBODY;
};
