#pragma once

#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/physics_server2d_extension.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

class Box2DPhysicsServer2D : public PhysicsServer2DExtension {
	GDCLASS(Box2DPhysicsServer2D, PhysicsServer2DExtension);

public:
	RID _space_create() override;
	void _space_set_active(const RID &p_space, bool p_active) override;
	bool _space_is_active(const RID &p_space) const override;

	PhysicsDirectSpaceState2D *_space_get_direct_state(const RID &p_space) override;

	void _area_set_param(const RID &p_area, AreaParameter p_param, const Variant &p_value);

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
	HashSet<Box2DSpace2D *> active_spaces;

    // soon:tm:
    //enki::TaskScheduler scheduler;
	//SampleTask tasks[maxTasks];
	//int32_t taskCount;
	//int threadCount;
};