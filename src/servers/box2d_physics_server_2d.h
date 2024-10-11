#pragma once

#include "../bodies/box2d_body_2d.h"
#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/physics_server2d_extension.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

class Box2DPhysicsServer2D : public PhysicsServer2DExtension {
	GDCLASS(Box2DPhysicsServer2D, PhysicsServer2DExtension);

public:
	// RID _world_boundary_shape_create() override;
	// RID _separation_ray_shape_create() override;
	// RID _segment_shape_create() override;
	RID _circle_shape_create() override;
	// RID _rectangle_shape_create() override;
	// RID _capsule_shape_create() override;
	// RID _convex_polygon_shape_create() override;
	// RID _concave_polygon_shape_create() override;
	void _shape_set_data(const RID &p_shape, const Variant &p_data) override;

	RID _space_create() override;
	void _space_set_active(const RID &p_space, bool p_active) override;
	bool _space_is_active(const RID &p_space) const override;

	PhysicsDirectSpaceState2D *_space_get_direct_state(const RID &p_space) override;

	void _area_set_param(const RID &p_area, AreaParameter p_param, const Variant &p_value);

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
	void _body_set_shape_as_one_way_collision(const RID &p_body, int32_t p_shape_idx, bool p_enable, double p_margin) override;
	void _body_remove_shape(const RID &p_body, int32_t p_shape_idx) override;
	void _body_clear_shapes(const RID &p_body) override;
	virtual void _body_attach_object_instance_id(const RID &p_body, uint64_t p_id) override;
	virtual uint64_t _body_get_object_instance_id(const RID &p_body) const override;
    virtual void _body_attach_canvas_instance_id(const RID &p_body, uint64_t p_id) override;
	virtual uint64_t _body_get_canvas_instance_id(const RID &p_body) const override;

	void _free_rid(const RID &p_rid) override;
	void _set_active(bool p_active) override;
	void _init() override;
	void _step(double p_step) override;
	void _sync() override {};
	void _flush_queries() override;
	void _end_sync() override {};
	void _finish() override {};
	bool _is_flushing_queries() const override;
	int32_t _get_process_info(PhysicsServer2D::ProcessInfo p_process_info) override;

private:
	static void _bind_methods();

	bool active = true;
	bool flushing_queries;
	mutable RID_PtrOwner<Box2DSpace2D> space_owner;
	mutable RID_PtrOwner<Box2DBody2D> body_owner;
	mutable RID_PtrOwner<Box2DShape2D> shape_owner;
	HashSet<Box2DSpace2D *> active_spaces;

	// soon:tm:
	//enki::TaskScheduler scheduler;
	//SampleTask tasks[maxTasks];
	//int32_t taskCount;
	//int threadCount;
};