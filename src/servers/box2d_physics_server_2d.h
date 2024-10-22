#pragma once

#include "../bodies/box2d_body_2d.h"
#include "../box2d_globals.h"
#include "../box2d_project_settings.h"
#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/physics_server2d_extension.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

class Box2DPhysicsServer2D : public PhysicsServer2DExtension {
	GDCLASS(Box2DPhysicsServer2D, PhysicsServer2DExtension);

public:
	RID _world_boundary_shape_create() override;
	RID _separation_ray_shape_create() override;
	RID _segment_shape_create() override;
	RID _circle_shape_create() override;
	RID _rectangle_shape_create() override;
	RID _capsule_shape_create() override;
	RID _convex_polygon_shape_create() override;
	RID _concave_polygon_shape_create() override;
	void _shape_set_data(const RID &p_shape, const Variant &p_data) override;

	RID _space_create() override;
	void _space_set_active(const RID &p_space, bool p_active) override;
	bool _space_is_active(const RID &p_space) const override;
	void _space_set_param(const RID &p_space, PhysicsServer2D::SpaceParameter p_param, float p_value) override {}
	//float _space_get_param(const RID &p_space, PhysicsServer2D::SpaceParameter p_param) const override {}
	PhysicsDirectSpaceState2D *_space_get_direct_state(const RID &p_space) override;
	void _space_set_debug_contacts(const RID &p_space, int32_t p_max_contacts) override;
	PackedVector2Array _space_get_contacts(const RID &p_space) const override;
	int32_t _space_get_contact_count(const RID &p_space) const override;

	// RID _area_create() override;
	// void _area_set_space(const RID &p_area, const RID &p_space) override;
	// RID _area_get_space(const RID &p_area) const override;
	// void _area_add_shape(const RID &p_area, const RID &p_shape, const Transform2D &p_transform, bool p_disabled) override;
	// void _area_set_shape(const RID &p_area, int32_t p_shape_idx, const RID &p_shape) override;
	// void _area_set_shape_transform(const RID &p_area, int32_t p_shape_idx, const Transform2D &p_transform) override;
	// void _area_set_shape_disabled(const RID &p_area, int32_t p_shape_idx, bool p_disabled) override;
	// int32_t _area_get_shape_count(const RID &p_area) const override;
	// RID _area_get_shape(const RID &p_area, int32_t p_shape_idx) const override;
	// Transform2D _area_get_shape_transform(const RID &p_area, int32_t p_shape_idx) const override;
	// void _area_remove_shape(const RID &p_area, int32_t p_shape_idx) override;
	// void _area_clear_shapes(const RID &p_area) override;
	// void _area_attach_object_instance_id(const RID &p_area, uint64_t p_id) override;
	// uint64_t _area_get_object_instance_id(const RID &p_area) const override;
	// void _area_attach_canvas_instance_id(const RID &p_area, uint64_t p_id) override;
	// uint64_t _area_get_canvas_instance_id(const RID &p_area) const override;
	void _area_set_param(const RID &p_area, PhysicsServer2D::AreaParameter p_param, const Variant &p_value) override;
	// void _area_set_transform(const RID &p_area, const Transform2D &p_transform) override;
	// Variant _area_get_param(const RID &p_area, PhysicsServer2D::AreaParameter p_param) const override;
	// Transform2D _area_get_transform(const RID &p_area) const override;
	// void _area_set_collision_layer(const RID &p_area, uint32_t p_layer) override;
	// uint32_t _area_get_collision_layer(const RID &p_area) const override;
	// void _area_set_collision_mask(const RID &p_area, uint32_t p_mask) override;
	// uint32_t _area_get_collision_mask(const RID &p_area) const override;
	// void _area_set_monitorable(const RID &p_area, bool p_monitorable) override;
	// void _area_set_pickable(const RID &p_area, bool p_pickable) override;
	// void _area_set_monitor_callback(const RID &p_area, const Callable &p_callback) override;
	// void _area_set_area_monitor_callback(const RID &p_area, const Callable &p_callback) override;

	RID _body_create() override;
	void _body_set_space(const RID &p_body, const RID &p_space) override;
	RID _body_get_space(const RID &p_body) const override;
	void _body_set_mode(const RID &p_body, PhysicsServer2D::BodyMode p_mode) override;
	PhysicsServer2D::BodyMode _body_get_mode(const RID &p_body) const override;

	void _body_add_shape(const RID &p_body, const RID &p_shape, const Transform2D &p_transform, bool p_disabled) override;
	void _body_set_shape(const RID &p_body, int32_t p_shape_idx, const RID &p_shape) override;
	void _body_set_shape_transform(const RID &p_body, int32_t p_shape_idx, const Transform2D &p_transform) override;
	int32_t _body_get_shape_count(const RID &p_body) const override;
	RID _body_get_shape(const RID &p_body, int32_t p_shape_idx) const override;
	Transform2D _body_get_shape_transform(const RID &p_body, int32_t p_shape_idx) const override;
	void _body_set_shape_disabled(const RID &p_body, int32_t p_shape_idx, bool p_disabled) override;
	void _body_set_shape_as_one_way_collision(const RID &p_body, int32_t p_shape_idx, bool p_enable, float p_margin) override;
	void _body_remove_shape(const RID &p_body, int32_t p_shape_idx) override;
	void _body_clear_shapes(const RID &p_body) override;
	void _body_attach_object_instance_id(const RID &p_body, uint64_t p_id) override;
	uint64_t _body_get_object_instance_id(const RID &p_body) const override;
	void _body_attach_canvas_instance_id(const RID &p_body, uint64_t p_id) override;
	uint64_t _body_get_canvas_instance_id(const RID &p_body) const override;

	void _body_set_continuous_collision_detection_mode(const RID &p_body, PhysicsServer2D::CCDMode p_mode) override;
	PhysicsServer2D::CCDMode _body_get_continuous_collision_detection_mode(const RID &p_body) const override;
	void _body_set_collision_layer(const RID &p_body, uint32_t p_layer) override;
	uint32_t _body_get_collision_layer(const RID &p_body) const override;
	void _body_set_collision_mask(const RID &p_body, uint32_t p_mask) override;
	uint32_t _body_get_collision_mask(const RID &p_body) const override;
	//  void _body_set_collision_priority(const RID &p_body, double p_priority) override;
	//  double _body_get_collision_priority(const RID &p_body) const override;
	void _body_set_param(const RID &p_body, PhysicsServer2D::BodyParameter p_param, const Variant &p_value) override;
	Variant _body_get_param(const RID &p_body, PhysicsServer2D::BodyParameter p_param) const override;
	void _body_reset_mass_properties(const RID &p_body) override;
	void _body_set_state(const RID &p_body, PhysicsServer2D::BodyState p_state, const Variant &p_value) override;
	Variant _body_get_state(const RID &p_body, PhysicsServer2D::BodyState p_state) const override;

	void _body_apply_central_impulse(const RID &p_body, const Vector2 &p_impulse) override;
	void _body_apply_torque_impulse(const RID &p_body, float p_impulse) override;
	void _body_apply_impulse(const RID &p_body, const Vector2 &p_impulse, const Vector2 &p_position) override;
	void _body_apply_central_force(const RID &p_body, const Vector2 &p_force) override;
	void _body_apply_force(const RID &p_body, const Vector2 &p_force, const Vector2 &p_position) override;
	void _body_apply_torque(const RID &p_body, float p_torque) override;
	// virtual void _body_add_constant_central_force(const RID &p_body, const Vector2 &p_force) override;
	// virtual void _body_add_constant_force(const RID &p_body, const Vector2 &p_force, const Vector2 &p_position) override;
	// virtual void _body_add_constant_torque(const RID &p_body, double p_torque) override;
	// virtual void _body_set_constant_force(const RID &p_body, const Vector2 &p_force) override;
	Vector2 _body_get_constant_force(const RID &p_body) const override { return Vector2(); }
	// virtual void _body_set_constant_torque(const RID &p_body, double p_torque) override;
	float _body_get_constant_torque(const RID &p_body) const override { return 0.0; }
	// virtual void _body_set_axis_velocity(const RID &p_body, const Vector2 &p_axis_velocity) override;
	// virtual void _body_add_collision_exception(const RID &p_body, const RID &p_excepted_body) override;
	// virtual void _body_remove_collision_exception(const RID &p_body, const RID &p_excepted_body) override;
	// virtual TypedArray<RID> _body_get_collision_exceptions(const RID &p_body) const override;
	// virtual void _body_set_max_contacts_reported(const RID &p_body, int32_t p_amount) override;
	// virtual int32_t _body_get_max_contacts_reported(const RID &p_body) const;
	// virtual void _body_set_contacts_reported_depth_threshold(const RID &p_body, double p_threshold) override;
	// virtual double _body_get_contacts_reported_depth_threshold(const RID &p_body) const override;
	// virtual void _body_set_omit_force_integration(const RID &p_body, bool p_enable) override;
	// virtual bool _body_is_omitting_force_integration(const RID &p_body) const override;
	void _body_set_state_sync_callback(const RID &p_body, const Callable &p_callable) override;
	void _body_set_force_integration_callback(const RID &p_body, const Callable &p_callable, const Variant &p_userdata) override {};
	// virtual bool _body_collide_shape(const RID &p_body, int32_t p_body_shape, const RID &p_shape, const Transform2D &p_shape_xform, const Vector2 &p_motion, void *p_results, int32_t p_result_max, int32_t *p_result_count) override;
	void _body_set_pickable(const RID &p_body, bool p_pickable) override {};

	void _free_rid(const RID &p_rid) override;
	void _set_active(bool p_active) override;
	void _init() override;
	void _step(float p_step) override;
	void _sync() override {};
	void _flush_queries() override;
	void _end_sync() override {};
	void _finish() override;
	bool _is_flushing_queries() const override;
	int32_t _get_process_info(PhysicsServer2D::ProcessInfo p_process_info) override;

	Box2DPhysicsServer2D();
	~Box2DPhysicsServer2D();
	static Box2DPhysicsServer2D *get_singleton();
	Box2DShape2D *get_shape(const RID &p_rid) const { return shape_owner.get_or_null(p_rid); }

private:
	static void _bind_methods();

	bool active = true;
	bool flushing_queries;
	mutable RID_PtrOwner<Box2DSpace2D> space_owner;
	mutable RID_PtrOwner<Box2DBody2D> body_owner;
	mutable RID_PtrOwner<Box2DShape2D> shape_owner;
	HashSet<Box2DSpace2D *> active_spaces;

	static Box2DPhysicsServer2D *singleton;
};