#pragma once

#include "../box2d_globals.h"
#include "../box2d_project_settings.h"
#include "../shapes/box2d_shape_2d.h"
#include "../shapes/box2d_shape_instance.h"
#include "../spaces/box2d_query.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

class Box2DSpace2D;
class Box2DArea2D;
class Box2DBody2D;

class Box2DCollisionObject2D {
public:
	enum Type {
		RIGIDBODY,
		AREA,
	};

	explicit Box2DCollisionObject2D(Type p_type);

	Type get_type() const { return type; }

	bool is_area() const { return type == Type::AREA; }
	bool is_body() const { return type == Type::RIGIDBODY; }

	Box2DArea2D *as_area() { return is_area() ? reinterpret_cast<Box2DArea2D *>(this) : nullptr; }
	Box2DBody2D *as_body() { return is_body() ? reinterpret_cast<Box2DBody2D *>(this) : nullptr; }

	const Box2DArea2D *as_area() const { return is_area() ? reinterpret_cast<const Box2DArea2D *>(this) : nullptr; }
	const Box2DBody2D *as_body() const { return is_body() ? reinterpret_cast<const Box2DBody2D *>(this) : nullptr; }

	void set_free();
	bool is_freed() const { return _is_freed; }

	void destroy_body();

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }

	void set_space(Box2DSpace2D *p_space);
	Box2DSpace2D *get_space() const { return space; }
	bool in_space() const { return space; }

	void set_mode(PhysicsServer2D::BodyMode p_mode);
	PhysicsServer2D::BodyMode get_mode() const { return mode; }
	bool is_dynamic() const { return mode > PhysicsServer2D::BODY_MODE_KINEMATIC; }

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
	void shape_updated(Box2DShape2D *p_shape);
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

	virtual void shapes_changed() {};

	struct CharacterCollideContext {
		b2ShapeId shape_id;
		b2Transform transform;
		Box2DShapePrimitive shape;
		LocalVector<CharacterCollideResult> &results;
	};

	struct CharacterCastContext {
		b2ShapeId shape_id;
		b2Transform transform;
		Box2DShapePrimitive shape;
		CharacterCastResult &result;
		Vector2 motion;
	};

	int character_collide(const Transform2D &p_from, float p_margin, LocalVector<CharacterCollideResult> &p_results);
	CharacterCastResult character_cast(const Transform2D &p_from, float p_margin, Vector2 p_motion);

protected:
	void build_shape(Box2DShapeInstance &p_shape, bool p_shapes_changed = true);
	void rebuild_all_shapes();

	virtual uint64_t modify_mask_bits(uint32_t p_mask) { return p_mask; }
	virtual uint64_t modify_layer_bits(uint32_t p_layer) { return p_layer; }

	virtual void body_created() {};
	virtual void body_destroyed() {};
	virtual void on_shape_destroy(Box2DShapeInstance &p_shape) {};

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

	bool _is_freed = false;
	Type type = Type::RIGIDBODY;
};
