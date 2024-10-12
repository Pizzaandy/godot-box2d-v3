#include "box2d_physics_server_2d.h"
#include "../shapes/box2d_circle_shape_2d.h"

#define FLUSH_QUERY_CHECK(m_object) \
	ERR_FAIL_COND_MSG(m_object->get_space() && flushing_queries, "Can't change this state while flushing queries. Use call_deferred() or set_deferred() to change monitoring state instead.");

void Box2DPhysicsServer2D::_bind_methods() {}

// Shape API
RID Box2DPhysicsServer2D::_circle_shape_create() {
	Box2DCircleShape2D *circle_shape = memnew(Box2DCircleShape2D);
	RID rid = shape_owner.make_rid(circle_shape);
	circle_shape->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_shape_set_data(const RID &p_shape, const Variant &p_data) {
	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);

	shape->set_data(p_data);
}

// Space API
RID Box2DPhysicsServer2D::_space_create() {
	Box2DSpace2D *space = memnew(Box2DSpace2D);
	RID rid = space_owner.make_rid(space);
	space->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_space_set_active(const RID &p_space, bool p_active) {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL(space);

	if (p_active) {
		active_spaces.insert(space);
	} else {
		active_spaces.erase(space);
	}
}

bool Box2DPhysicsServer2D::_space_is_active(const RID &p_space) const {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, false);

	return active_spaces.has(space);
}

PhysicsDirectSpaceState2D *Box2DPhysicsServer2D::_space_get_direct_state(const RID &p_space) {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, {});

	return space->get_direct_state();
}

void Box2DPhysicsServer2D::_area_set_param(
		const RID &p_area,
		AreaParameter p_param,
		const Variant &p_value) {
	RID area_rid = p_area;

	// if (space_owner.owns(area_rid)) {
	// 	const Box2DSpace2D* space = space_owner.get_or_null(area_rid);
	// 	area_rid = space->get_default_area()->get_rid();
	// }

	// Box2DArea2D* area = area_owner.get_or_null(area_rid);
	// ERR_FAIL_NULL(area);
	// area->set_param(p_param, p_value);
}

// Body API
RID Box2DPhysicsServer2D::_body_create() {
	Box2DBody2D *body = memnew(Box2DBody2D);
	RID rid = body_owner.make_rid(body);
	body->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_body_set_space(const RID &p_body, const RID &p_space) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	Box2DSpace2D *space = nullptr;
	if (p_space.is_valid()) {
		space = space_owner.get_or_null(p_space);
		ERR_FAIL_COND(!space);
	}

	body->set_space(space);
}

RID Box2DPhysicsServer2D::_body_get_space(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, RID());

	Box2DSpace2D *space = body->get_space();
	if (!space) {
		return RID();
	}

	return space->get_rid();
}

void Box2DPhysicsServer2D::_body_set_mode(const RID &p_body, BodyMode p_mode) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	body->set_mode(p_mode);
}

PhysicsServer2D::BodyMode Box2DPhysicsServer2D::_body_get_mode(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, BODY_MODE_STATIC);

	return body->get_mode();
}

void Box2DPhysicsServer2D::_body_add_shape(const RID &p_body, const RID &p_shape, const Transform2D &p_transform, bool p_disabled) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!shape);

	body->add_shape(shape, p_transform, p_disabled);
}

void Box2DPhysicsServer2D::_body_set_shape(const RID &p_body, int32_t p_shape_idx, const RID &p_shape) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_COND(!shape);

	body->set_shape(p_shape_idx, shape);
}

void Box2DPhysicsServer2D::_body_set_shape_transform(const RID &p_body, int32_t p_shape_idx, const Transform2D &p_transform) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	body->set_shape_transform(p_shape_idx, p_transform);
}

int32_t Box2DPhysicsServer2D::_body_get_shape_count(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, 0);

	return body->get_shape_count();
}

RID Box2DPhysicsServer2D::_body_get_shape(const RID &p_body, int32_t p_shape_idx) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, RID());

	return body->get_shape_rid(p_shape_idx);
}

Transform2D Box2DPhysicsServer2D::_body_get_shape_transform(const RID &p_body, int32_t p_shape_idx) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, Transform2D());

	return body->get_shape_transform(p_shape_idx);
}

void Box2DPhysicsServer2D::_body_set_shape_disabled(const RID &p_body, int32_t p_shape_idx, bool p_disabled) {}

void Box2DPhysicsServer2D::_body_set_shape_as_one_way_collision(const RID &p_body, int32_t p_shape_idx, bool p_enable, double p_margin) {}

void Box2DPhysicsServer2D::_body_remove_shape(const RID &p_body, int32_t p_shape_idx) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	body->remove_shape(p_shape_idx);
}

void Box2DPhysicsServer2D::_body_clear_shapes(const RID &p_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	while (body->get_shape_count()) {
		body->remove_shape(0);
	}
}

void Box2DPhysicsServer2D::_body_attach_object_instance_id(const RID &p_body, uint64_t p_id) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	return body->set_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_body_get_object_instance_id(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, ObjectID());

	return body->get_instance_id();
}

void Box2DPhysicsServer2D::_body_attach_canvas_instance_id(const RID &p_body, uint64_t p_id) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	return body->set_canvas_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_body_get_canvas_instance_id(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, ObjectID());

	return body->get_canvas_instance_id();
}

void Box2DPhysicsServer2D::_body_set_state(const RID &p_body, PhysicsServer2D::BodyState p_state, const Variant &p_value) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	switch (p_state) {
		case PhysicsServer2D::BodyState::BODY_STATE_TRANSFORM:
			body->set_transform(p_value);
			break;
		case PhysicsServer2D::BodyState::BODY_STATE_LINEAR_VELOCITY:
			body->set_linear_velocity(p_value);
			break;
		case PhysicsServer2D::BodyState::BODY_STATE_ANGULAR_VELOCITY:
			body->set_angular_velocity(p_value);
			break;
		case PhysicsServer2D::BodyState::BODY_STATE_SLEEPING:
			return;
		case PhysicsServer2D::BodyState::BODY_STATE_CAN_SLEEP:
			return;
	}
}

Variant Box2DPhysicsServer2D::_body_get_state(const RID &p_body, PhysicsServer2D::BodyState p_state) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND_V(!body, Variant());

	switch (p_state) {
		case PhysicsServer2D::BodyState::BODY_STATE_TRANSFORM:
			return body->get_transform();
		case PhysicsServer2D::BodyState::BODY_STATE_LINEAR_VELOCITY:
			return body->get_linear_velocity();
		case PhysicsServer2D::BodyState::BODY_STATE_ANGULAR_VELOCITY:
			return body->get_angular_velocity();
		case PhysicsServer2D::BodyState::BODY_STATE_SLEEPING:
			return body->is_sleeping();
		case PhysicsServer2D::BodyState::BODY_STATE_CAN_SLEEP:
			return true;
	}

	return Variant();
}

void Box2DPhysicsServer2D::_body_set_state_sync_callback(const RID &p_body, const Callable &p_callable) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_COND(!body);

	return body->set_state_sync_callback(p_callable);
}

void Box2DPhysicsServer2D::_free_rid(const RID &p_rid) {
	if (shape_owner.owns(p_rid)) {
		Box2DShape2D *shape = shape_owner.get_or_null(p_rid);
		shape_owner.free(p_rid);
		memdelete(shape);
	} else if (body_owner.owns(p_rid)) {
		Box2DBody2D *body = body_owner.get_or_null(p_rid);
		body->set_space(nullptr);
		body_owner.free(p_rid);
		memdelete(body);
	}
	// else if (area_owner.owns(p_rid)) {
	// 	Box2DArea2D *area = area_owner.get_or_null(p_rid);

	// 	area->set_space(nullptr);

	// 	while (area->get_shape_count()) {
	// 		area->remove_shape(0);
	// 	}

	// 	area_owner.free(p_rid);
	// 	memdelete(area);
	// }
	else if (space_owner.owns(p_rid)) {
		Box2DSpace2D *space = space_owner.get_or_null(p_rid);
		active_spaces.erase(space);
		space_owner.free(p_rid);
		memdelete(space);
	}
	// else if (joint_owner.owns(p_rid)) {
	// 	Box2DJoint2D *joint = joint_owner.get_or_null(p_rid);

	// 	joint_owner.free(p_rid);
	// 	memdelete(joint);
	// }
	else {
		ERR_FAIL_MSG("Attempted to free invalid RID.");
	}
}

void Box2DPhysicsServer2D::_set_active(bool p_active) {
	active = p_active;
}

void Box2DPhysicsServer2D::_init() {
	b2SetLengthUnitsPerMeter(100.0);
}

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

	for (Box2DSpace2D *space : active_spaces) {
		space->call_queries();
	}

	flushing_queries = false;
}

void Box2DPhysicsServer2D::_finish() {}

bool Box2DPhysicsServer2D::_is_flushing_queries() const {
	return flushing_queries;
}

int32_t Box2DPhysicsServer2D::_get_process_info([[maybe_unused]] ProcessInfo p_process_info) {
	return 0;
}