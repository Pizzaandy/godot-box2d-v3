#pragma once

#include "../box2d_globals.h"
#include "../box2d_project_settings.h"
#include "../shapes/box2d_concave_polygon_shape_2d.h"
#include "../shapes/box2d_shape_2d.h"
#include "../spaces/box2d_space_2d.h"
#include "box2d_direct_body_state_2d.h"
#include "chain_segment_range.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

class Box2DDirectBodyState2D;

class Box2DBody2D {
public:
	struct Shape {
		int index = -1;
		Box2DShape2D *shape = nullptr;
		Box2DShape2D::ShapeID shape_id;
		Transform2D transform;
		bool disabled = false;
		bool one_way_collision = false;
		real_t one_way_collision_margin = 0.0;

		void set_user_data(b2ShapeId id) {
			b2Shape_SetUserData(id, this);
		}

		void build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) {
			destroy();

			if (disabled) {
				return;
			}

			ERR_FAIL_COND(!shape);
			shape_id = shape->build(p_body, p_transform, p_shape_def);

			if (shape_id.type == Box2DShape2D::ShapeID::CHAIN) {
				ChainSegmentRange range(shape_id.chain_id);
				for (b2ShapeId id : range) {
					b2Shape_SetUserData(id, this);
				}
			} else {
				b2Shape_SetUserData(shape_id.shape_id, this);
			}
		}

		void destroy() {
			if (b2Shape_IsValid(shape_id.shape_id)) {
				b2DestroyShape(shape_id.shape_id, false);
			} else if (b2Chain_IsValid(shape_id.chain_id)) {
				b2DestroyChain(shape_id.chain_id);
			}
			shape_id = {};
		}

		bool exists() {
			return !disabled && shape_id.is_valid();
		}
	};

	Box2DBody2D();
	~Box2DBody2D();

	void destroy_body();

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }

	void set_space(Box2DSpace2D *p_space);
	Box2DSpace2D *get_space() { return space; }

	void set_mode(PhysicsServer2D::BodyMode p_mode);
	PhysicsServer2D::BodyMode get_mode() { return mode; }

	void set_bullet(bool p_bullet);
	bool get_bullet() { return is_bullet; }

	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() { return layer; }
	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() { return mask; }

	void set_transform(Transform2D p_transform, bool p_move_kinematic = false);
	Transform2D get_transform() { return current_transform; }

	void apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position);
	void apply_impulse_center(const Vector2 &p_impulse);
	void apply_torque_impulse(float p_impulse);
	void set_linear_velocity(const Vector2 &p_velocity);
	Vector2 get_linear_velocity() const;
	void set_angular_velocity(float p_velocity);
	float get_angular_velocity() const;
	bool is_sleeping() { return sleeping; }

	void sync_state(b2Transform p_transform, bool is_sleeping);

	void add_shape(Box2DShape2D *p_shape, Transform2D p_transform, bool p_disabled);
	void set_shape(int p_index, Box2DShape2D *p_shape);
	void remove_shape(int p_index);
	int32_t get_shape_count() { return shapes.size(); }
	void set_shape_transform(int p_index, Transform2D p_transform);
	Transform2D get_shape_transform(int p_index);
	RID get_shape_rid(int p_index);

	void set_instance_id(const ObjectID &p_instance_id) { instance_id = p_instance_id; }
	ObjectID get_instance_id() const { return instance_id; }

	void set_canvas_instance_id(const ObjectID &p_canvas_instance_id) { canvas_instance_id = p_canvas_instance_id; }
	ObjectID get_canvas_instance_id() const { return canvas_instance_id; }

	Box2DDirectBodyState2D *get_direct_state();
	void set_state_sync_callback(const Callable &p_callable) { body_state_callback = p_callable; }

private:
	void build_shape(Shape &p_shape);

	bool is_locked() const { return !body_exists || space->locked; }

	RID rid;
	ObjectID instance_id;
	ObjectID canvas_instance_id;
	Callable body_state_callback;
	Transform2D current_transform;
	PhysicsServer2D::BodyMode mode;
	LocalVector<Shape> shapes;

	Box2DDirectBodyState2D *direct_state = nullptr;
	Box2DSpace2D *space = nullptr;

	uint32_t layer;
	uint32_t mask;

	b2BodyDef body_def = b2DefaultBodyDef();
	b2ShapeDef shape_def = b2DefaultShapeDef();
	b2BodyId body_id = b2_nullBodyId;

	bool sleeping = false;
	bool is_area = false;
	bool is_bullet = false;
	bool body_exists = false;
};
