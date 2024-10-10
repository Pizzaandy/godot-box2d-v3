#include "box2d_physics_server_2d.h"

void Box2DPhysicsServer2D::_bind_methods() {}

RID Box2DPhysicsServer2D::_space_create() {
	Box2DSpace2D *space = memnew(Box2DSpace2D);
	RID rid = space_owner.make_rid(space);
	space->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_space_set_active(const RID& p_space, bool p_active) {
	Box2DSpace2D* space = space_owner.get_or_null(p_space);
    ERR_FAIL_NULL(space);

    if (p_active) {
		active_spaces.insert(space);
	} else {
		active_spaces.erase(space);
	}
}

bool Box2DPhysicsServer2D::_space_is_active(const RID& p_space) const {
    Box2DSpace2D* space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, false);

	return active_spaces.has(space);
}


PhysicsDirectSpaceState2D* Box2DPhysicsServer2D::_space_get_direct_state(const RID& p_space) {
    Box2DSpace2D* space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, {});

	return space->get_direct_state();
}

void Box2DPhysicsServer2D::_area_set_param(
	const RID& p_area,
	AreaParameter p_param,
	const Variant& p_value
) {
	RID area_rid = p_area;

	// if (space_owner.owns(area_rid)) {
	// 	const Box2DSpace2D* space = space_owner.get_or_null(area_rid);
	// 	area_rid = space->get_default_area()->get_rid();
	// }

	// Box2DArea2D* area = area_owner.get_or_null(area_rid);
	// ERR_FAIL_NULL(area);
	// area->set_param(p_param, p_value);
}


void Box2DPhysicsServer2D::_set_active(bool p_active) {
	active = p_active;
}

void Box2DPhysicsServer2D::_init() {}

void Box2DPhysicsServer2D::_step(double p_step) {
	if (!active) {
		return;
	}

	for (Box2DSpace2D *active_space : active_spaces) {
		active_space->step((float)p_step);
	}
}

void Box2DPhysicsServer2D::_flush_queries() {
    if (!active) {
		return;
	}

    flushing_queries = true;

    // for (Box2DSpace2D* space : active_spaces) {
	// 	space->call_queries();
	// }

	flushing_queries = false;
}

bool Box2DPhysicsServer2D::_is_flushing_queries() const {
	return flushing_queries;
}

int32_t Box2DPhysicsServer2D::_get_process_info([[maybe_unused]] ProcessInfo p_process_info) {
	return 0;
}