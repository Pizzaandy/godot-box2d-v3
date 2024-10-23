#pragma once

#include "../box2d_globals.h"
#include "../box2d_project_settings.h"
#include "../shapes/box2d_concave_polygon_shape_2d.h"
#include "../shapes/box2d_shape_2d.h"
#include "../shapes/box2d_shape_instance.h"
#include "../spaces/box2d_space_2d.h"
#include "body_shape_range.h"
#include "box2d_direct_body_state_2d.h"
#include "chain_segment_range.h"
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/templates/local_vector.hpp>

class Box2DDirectBodyState2D;

class Box2DBody2D {
public:
	Box2DBody2D();
	~Box2DBody2D();

	void queue_delete();
	void destroy_body();

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }

	void set_space(Box2DSpace2D *p_space);
	Box2DSpace2D *get_space() const { return space; }

	void set_mode(PhysicsServer2D::BodyMode p_mode);
	PhysicsServer2D::BodyMode get_mode() const { return mode; }

	void set_bullet(bool p_bullet);
	bool get_bullet() const { return is_bullet; }

	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() { return layer; }
	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() { return mask; }

	void set_transform(const Transform2D &p_transform, bool p_move_kinematic = false);
	Transform2D get_transform() const { return current_transform; }

	float get_bounce() const { return bounce; }
	void set_bounce(float p_bounce);
	float get_friction() const { return friction; }
	void set_friction(float p_friction);
	void reset_mass();
	float get_mass() const { return mass_data.mass; }
	void set_mass(float p_mass);
	float get_inertia() const { return mass_data.rotationalInertia; }
	void set_inertia(float p_inertia);
	Vector2 get_center_of_mass() const { return to_godot(mass_data.center); }
	void set_center_of_mass(const Vector2 &p_center);
	float get_gravity_scale() const { return body_def.gravityScale; }
	void set_gravity_scale(float p_scale);
	float get_linear_damping() const { return body_def.linearDamping; }
	void set_linear_damping(float p_damping);
	float get_angular_damping() const { return body_def.angularDamping; }
	void set_angular_damping(float p_damping);

	void apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position);
	void apply_impulse_center(const Vector2 &p_impulse);
	void apply_torque(float p_torque);
	void apply_torque_impulse(float p_impulse);
	void apply_force(const Vector2 &p_force, const Vector2 &p_position);
	void apply_force_center(const Vector2 &p_force);

	void set_linear_velocity(const Vector2 &p_velocity);
	Vector2 get_linear_velocity() const;
	Vector2 get_linear_velocity_at_point(const Vector2 &p_point) const;
	void set_angular_velocity(float p_velocity);
	float get_angular_velocity() const;
	bool is_sleeping() { return sleeping; }

	void sync_state(const b2Transform &p_transform, bool is_sleeping);

	void add_shape(Box2DShape2D *p_shape, const Transform2D &p_transform, bool p_disabled);
	void set_shape(int p_index, Box2DShape2D *p_shape);
	void remove_shape(int p_index);
	int32_t get_shape_count() const { return shapes.size(); }
	void set_shape_transform(int p_index, const Transform2D &p_transform);
	Transform2D get_shape_transform(int p_index) const;
	RID get_shape_rid(int p_index) const;

	void add_collision_exception(RID p_rid);
	void remove_collision_exception(RID p_rid);
	bool is_collision_exception(RID p_rid) const;

	void set_presolve_enabled(bool p_enabled);

	void set_instance_id(const ObjectID &p_instance_id) { instance_id = p_instance_id; }
	ObjectID get_instance_id() const { return instance_id; }

	void set_canvas_instance_id(const ObjectID &p_canvas_instance_id) { canvas_instance_id = p_canvas_instance_id; }
	ObjectID get_canvas_instance_id() const { return canvas_instance_id; }

	Box2DDirectBodyState2D *get_direct_state();
	void set_state_sync_callback(const Callable &p_callable) { body_state_callback = p_callable; }

private:
	void build_shape(Box2DShapeInstance *p_shape, bool p_update_mass);
	void rebuild_all_shapes();
	void update_mass(bool p_recompute_from_shapes);

	bool is_locked() const { return !body_exists || !space || space->locked; }

	Box2DDirectBodyState2D *direct_state = nullptr;
	Box2DSpace2D *space = nullptr;

	RID rid;
	ObjectID instance_id;
	ObjectID canvas_instance_id;
	Callable body_state_callback;
	Transform2D current_transform;
	PhysicsServer2D::BodyMode mode;
	LocalVector<Box2DShapeInstance *> shapes;
	HashSet<RID> exceptions;

	uint32_t layer;
	uint32_t mask;

	float friction = 0.6;
	float bounce = 0.0;
	float mass = 1.0;

	b2MassData mass_data = b2MassData{ 0.0, b2Vec2{ 0 }, 0.0 };
	bool override_center_of_mass = false;
	Vector2 center_of_mass = Vector2();
	bool override_inertia = false;
	float inertia = 0.0;

	b2BodyDef body_def = b2DefaultBodyDef();
	b2ShapeDef shape_def = b2DefaultShapeDef();
	b2BodyId body_id = b2_nullBodyId;

	bool sleeping = false;
	bool is_area = false;
	bool is_bullet = false;
	bool body_exists = false;
};
