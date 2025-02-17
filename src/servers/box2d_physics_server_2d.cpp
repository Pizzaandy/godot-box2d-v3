#include "box2d_physics_server_2d.h"

#include "../shapes/box2d_capsule_shape_2d.h"
#include "../shapes/box2d_circle_shape_2d.h"
#include "../shapes/box2d_concave_polygon_shape_2d.h"
#include "../shapes/box2d_convex_polygon_shape_2d.h"
#include "../shapes/box2d_rectangle_shape_2d.h"
#include "../shapes/box2d_segment_shape_2d.h"
#include "../shapes/box2d_separation_ray_shape_2d.h"

#include "../joints/box2d_damped_spring_joint_2d.h"
#include "../joints/box2d_groove_joint_2d.h"
#include "../joints/box2d_pin_joint_2d.h"

namespace {
constexpr char PHYSICS_SERVER_NAME[] = "Box2DPhysicsServer2D";
}

void Box2DPhysicsServer2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("space_get_body_move_events", "space"), &Box2DPhysicsServer2D::space_get_body_move_events);
	ClassDB::bind_method(D_METHOD("body_set_user_data", "body", "data"), &Box2DPhysicsServer2D::body_set_user_data);
	ClassDB::bind_method(D_METHOD("body_get_user_data", "body"), &Box2DPhysicsServer2D::body_get_user_data);
}

Box2DPhysicsServer2D::Box2DPhysicsServer2D() {
	Engine *engine = Engine::get_singleton();

	if (engine->has_singleton(PHYSICS_SERVER_NAME)) {
		engine->unregister_singleton(PHYSICS_SERVER_NAME);
	}

	engine->register_singleton(PHYSICS_SERVER_NAME, this);
}

Box2DPhysicsServer2D::~Box2DPhysicsServer2D() {
	Engine::get_singleton()->unregister_singleton(PHYSICS_SERVER_NAME);
}

Box2DPhysicsServer2D *Box2DPhysicsServer2D::get_singleton() {
	static auto *instance = dynamic_cast<Box2DPhysicsServer2D *>(PhysicsServer2D::get_singleton());
	return instance;
}

// Shape API
RID Box2DPhysicsServer2D::_world_boundary_shape_create() {
	ERR_PRINT_ONCE("Box2D: World boundary shape is not yet supported. This feature is planned for v3.1.");
	return RID();
}

RID Box2DPhysicsServer2D::_separation_ray_shape_create() {
	Box2DSeparationRayShape2D *shape = memnew(Box2DSeparationRayShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_segment_shape_create() {
	Box2DSegmentShape2D *shape = memnew(Box2DSegmentShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_circle_shape_create() {
	Box2DCircleShape2D *shape = memnew(Box2DCircleShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_rectangle_shape_create() {
	Box2DRectangleShape2D *shape = memnew(Box2DRectangleShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_capsule_shape_create() {
	Box2DCapsuleShape2D *shape = memnew(Box2DCapsuleShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_convex_polygon_shape_create() {
	Box2DConvexPolygonShape2D *shape = memnew(Box2DConvexPolygonShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

RID Box2DPhysicsServer2D::_concave_polygon_shape_create() {
	Box2DConcavePolygonShape2D *shape = memnew(Box2DConcavePolygonShape2D);
	RID rid = shape_owner.make_rid(shape);
	shape->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_shape_set_data(const RID &p_shape, const Variant &p_data) {
	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);
	shape->set_data(p_data);
}

PhysicsServer2D::ShapeType Box2DPhysicsServer2D::_shape_get_type(const RID &p_shape) const {
	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL_V(shape, PhysicsServer2D::ShapeType::SHAPE_CUSTOM);
	return shape->get_type();
}

Variant Box2DPhysicsServer2D::_shape_get_data(const RID &p_shape) const {
	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL_V(shape, Variant());
	return shape->get_data();
}

bool Box2DPhysicsServer2D::_shape_collide(
		const RID &p_shape_A,
		const Transform2D &p_xform_A,
		const Vector2 &p_motion_A,
		const RID &p_shape_B,
		const Transform2D &p_xform_B,
		const Vector2 &p_motion_B,
		void *p_results,
		int32_t p_result_max,
		int32_t *p_result_count) {
	return false;
}

// Space API
RID Box2DPhysicsServer2D::_space_create() {
	Box2DSpace2D *space = memnew(Box2DSpace2D);
	RID rid = space_owner.make_rid(space);
	space->set_rid(rid);

	RID default_area_rid = area_create();
	Box2DArea2D *default_area = area_owner.get_or_null(default_area_rid);
	ERR_FAIL_NULL_V(default_area, RID());
	space->set_default_area(default_area);
	default_area->set_space(space);

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

void Box2DPhysicsServer2D::_space_set_param(const RID &p_space, PhysicsServer2D::SpaceParameter p_param, float p_value) {
	WARN_PRINT_ONCE("Box2D: Godot space parameters are not supported. Any values set will be ignored.");
}

float Box2DPhysicsServer2D::_space_get_param(const RID &p_space, PhysicsServer2D::SpaceParameter p_param) const {
	WARN_PRINT_ONCE("Box2D: Godot space parameters are not supported.");
	return 0.0;
}

PhysicsDirectSpaceState2D *Box2DPhysicsServer2D::_space_get_direct_state(const RID &p_space) {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, {});
	return space->get_direct_state();
}

void Box2DPhysicsServer2D::_space_set_debug_contacts(const RID &p_space, int32_t p_max_contacts) {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL(space);
	return space->set_max_debug_contacts(p_max_contacts);
}

PackedVector2Array Box2DPhysicsServer2D::_space_get_contacts(const RID &p_space) const {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, PackedVector2Array());
	return space->get_debug_contacts();
}

int32_t Box2DPhysicsServer2D::_space_get_contact_count(const RID &p_space) const {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, 0);
	return space->get_debug_contact_count();
}

Array Box2DPhysicsServer2D::space_get_body_move_events(const RID &p_space) const {
	Box2DSpace2D *space = space_owner.get_or_null(p_space);
	ERR_FAIL_NULL_V(space, {});
	ERR_FAIL_COND_V(space->locked, {});

	Array results;

	b2BodyEvents body_events = b2World_GetBodyEvents(space->get_world_id());

	for (int i = 0; i < body_events.moveCount; ++i) {
		const b2BodyMoveEvent *event = body_events.moveEvents + i;
		Box2DBody2D *body = static_cast<Box2DBody2D *>(event->userData);
		results.push_back(body->get_user_data());
		results.push_back(body->get_transform());
	}

	return results;
}

// Area API
RID Box2DPhysicsServer2D::_area_create() {
	Box2DArea2D *area = memnew(Box2DArea2D);
	RID rid = area_owner.make_rid(area);
	area->set_rid(rid);
	return rid;
}

void Box2DPhysicsServer2D::_area_set_space(const RID &p_area, const RID &p_space) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);

	Box2DSpace2D *space = nullptr;
	if (p_space.is_valid()) {
		space = space_owner.get_or_null(p_space);
		ERR_FAIL_NULL(space);
	}

	area->set_space(space);
}

RID Box2DPhysicsServer2D::_area_get_space(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, RID());

	Box2DSpace2D *space = area->get_space();
	if (!space) {
		return RID();
	}

	return space->get_rid();
}

void Box2DPhysicsServer2D::_area_add_shape(const RID &p_area, const RID &p_shape, const Transform2D &p_transform, bool p_disabled) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);

	area->add_shape(shape, p_transform, p_disabled);
}

void Box2DPhysicsServer2D::_area_set_shape(const RID &p_area, int32_t p_shape_idx, const RID &p_shape) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);

	area->set_shape(p_shape_idx, shape);
}

void Box2DPhysicsServer2D::_area_set_shape_transform(const RID &p_area, int32_t p_shape_idx, const Transform2D &p_transform) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_shape_transform(p_shape_idx, p_transform);
}

void Box2DPhysicsServer2D::_area_set_shape_disabled(const RID &p_area, int32_t p_shape_idx, bool p_disabled) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_shape_disabled(p_shape_idx, p_disabled);
}

int32_t Box2DPhysicsServer2D::_area_get_shape_count(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, 0);
	return area->get_shape_count();
}

RID Box2DPhysicsServer2D::_area_get_shape(const RID &p_area, int32_t p_shape_idx) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, RID());
	return area->get_shape_rid(p_shape_idx);
}

Transform2D Box2DPhysicsServer2D::_area_get_shape_transform(const RID &p_area, int32_t p_shape_idx) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, Transform2D());
	return area->get_shape_transform(p_shape_idx);
}

void Box2DPhysicsServer2D::_area_remove_shape(const RID &p_area, int32_t p_shape_idx) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->remove_shape(p_shape_idx);
}

void Box2DPhysicsServer2D::_area_clear_shapes(const RID &p_area) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->clear_shapes();
}

void Box2DPhysicsServer2D::_area_attach_object_instance_id(const RID &p_area, uint64_t p_id) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_area_get_object_instance_id(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, ObjectID());
	return area->get_canvas_instance_id();
}

void Box2DPhysicsServer2D::_area_attach_canvas_instance_id(const RID &p_area, uint64_t p_id) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_canvas_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_area_get_canvas_instance_id(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, ObjectID());
	return area->get_canvas_instance_id();
}

void Box2DPhysicsServer2D::_area_set_param(
		const RID &p_area,
		AreaParameter p_param,
		const Variant &p_value) {
	RID area_rid = p_area;

	if (space_owner.owns(area_rid)) {
		Box2DSpace2D *space = space_owner.get_or_null(area_rid);
		area_rid = space->get_default_area()->get_rid();
	}

	Box2DArea2D *area = area_owner.get_or_null(area_rid);
	ERR_FAIL_NULL(area);

	switch (p_param) {
		case AreaParameter::AREA_PARAM_GRAVITY_OVERRIDE_MODE:
			// TODO: add variant type checks
			area->set_gravity_override_mode((PhysicsServer2D::AreaSpaceOverrideMode)(int)p_value);
			break;
		case AreaParameter::AREA_PARAM_GRAVITY:
			area->set_gravity_strength(p_value);
			break;
		case AreaParameter::AREA_PARAM_GRAVITY_VECTOR:
			area->set_gravity_direction(p_value);
			break;
		case AreaParameter::AREA_PARAM_GRAVITY_IS_POINT:
			area->set_gravity_point_enabled(p_value);
			break;
		case AreaParameter::AREA_PARAM_GRAVITY_POINT_UNIT_DISTANCE:
			area->set_gravity_point_unit_distance(p_value);
			break;
		case AreaParameter::AREA_PARAM_LINEAR_DAMP_OVERRIDE_MODE:
			area->set_linear_damp_override_mode((PhysicsServer2D::AreaSpaceOverrideMode)(int)p_value);
			break;
		case AreaParameter::AREA_PARAM_LINEAR_DAMP:
			area->set_linear_damp(p_value);
			break;
		case AreaParameter::AREA_PARAM_ANGULAR_DAMP_OVERRIDE_MODE:
			area->set_angular_damp_override_mode((PhysicsServer2D::AreaSpaceOverrideMode)(int)p_value);
			break;
		case AreaParameter::AREA_PARAM_ANGULAR_DAMP:
			area->set_angular_damp(p_value);
			break;
		case AreaParameter::AREA_PARAM_PRIORITY:
			area->set_priority(p_value);
			break;
		default:
			break;
	}
}

void Box2DPhysicsServer2D::_area_set_transform(const RID &p_area, const Transform2D &p_transform) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_transform(p_transform);
}

Variant Box2DPhysicsServer2D::_area_get_param(const RID &p_area, PhysicsServer2D::AreaParameter p_param) const {
	RID area_rid = p_area;

	if (space_owner.owns(area_rid)) {
		Box2DSpace2D *space = space_owner.get_or_null(area_rid);
		area_rid = space->get_default_area()->get_rid();
	}

	Box2DArea2D *area = area_owner.get_or_null(area_rid);
	ERR_FAIL_NULL_V(area, Variant());

	switch (p_param) {
		case AreaParameter::AREA_PARAM_GRAVITY_OVERRIDE_MODE:
			return area->get_gravity_override_mode();
		case AreaParameter::AREA_PARAM_GRAVITY:
			return area->get_gravity_strength();
		case AreaParameter::AREA_PARAM_GRAVITY_VECTOR:
			return area->get_gravity_direction();
		case AreaParameter::AREA_PARAM_GRAVITY_IS_POINT:
			return area->get_gravity_point_enabled();
		case AreaParameter::AREA_PARAM_GRAVITY_POINT_UNIT_DISTANCE:
			return area->get_gravity_point_unit_distance();
		case AreaParameter::AREA_PARAM_LINEAR_DAMP_OVERRIDE_MODE:
			return area->get_linear_damp_override_mode();
		case AreaParameter::AREA_PARAM_LINEAR_DAMP:
			return area->get_linear_damp();
		case AreaParameter::AREA_PARAM_ANGULAR_DAMP_OVERRIDE_MODE:
			return area->get_angular_damp_override_mode();
		case AreaParameter::AREA_PARAM_ANGULAR_DAMP:
			return area->get_angular_damp();
		case AreaParameter::AREA_PARAM_PRIORITY:
			return area->get_priority();
		default:
			ERR_FAIL_V(Variant());
	}
}

Transform2D Box2DPhysicsServer2D::_area_get_transform(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, Transform2D());
	return area->get_transform();
}

void Box2DPhysicsServer2D::_area_set_collision_layer(const RID &p_area, uint32_t p_layer) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_collision_layer(p_layer);
}

uint32_t Box2DPhysicsServer2D::_area_get_collision_layer(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, 0);
	return area->get_collision_layer();
}

void Box2DPhysicsServer2D::_area_set_collision_mask(const RID &p_area, uint32_t p_mask) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_collision_mask(p_mask);
}

uint32_t Box2DPhysicsServer2D::_area_get_collision_mask(const RID &p_area) const {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL_V(area, 0);
	return area->get_collision_mask();
}

void Box2DPhysicsServer2D::_area_set_monitorable(const RID &p_area, bool p_monitorable) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	return area->set_monitorable(p_monitorable);
}

void Box2DPhysicsServer2D::_area_set_pickable(const RID &p_area, bool p_pickable) {
	// TODO: figure out what to do here
}

void Box2DPhysicsServer2D::_area_set_monitor_callback(const RID &p_area, const Callable &p_callback) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_body_monitor_callback(p_callback);
}

void Box2DPhysicsServer2D::_area_set_area_monitor_callback(const RID &p_area, const Callable &p_callback) {
	Box2DArea2D *area = area_owner.get_or_null(p_area);
	ERR_FAIL_NULL(area);
	area->set_area_monitor_callback(p_callback);
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
	ERR_FAIL_NULL(body);

	Box2DSpace2D *space = nullptr;
	if (p_space.is_valid()) {
		space = space_owner.get_or_null(p_space);
		ERR_FAIL_NULL(space);
	}

	body->set_space(space);
}

RID Box2DPhysicsServer2D::_body_get_space(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, RID());

	Box2DSpace2D *space = body->get_space();
	if (!space) {
		return RID();
	}

	return space->get_rid();
}

void Box2DPhysicsServer2D::_body_set_mode(const RID &p_body, BodyMode p_mode) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	body->set_mode(p_mode);
}

PhysicsServer2D::BodyMode Box2DPhysicsServer2D::_body_get_mode(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, BODY_MODE_STATIC);
	return body->get_mode();
}

void Box2DPhysicsServer2D::body_set_user_data(const RID &p_body, const Variant &p_variant) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_user_data(p_variant);
}

Variant Box2DPhysicsServer2D::body_get_user_data(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, Variant());
	return body->get_user_data();
}

void Box2DPhysicsServer2D::_body_add_shape(const RID &p_body, const RID &p_shape, const Transform2D &p_transform, bool p_disabled) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);

	body->add_shape(shape, p_transform, p_disabled);
}

void Box2DPhysicsServer2D::_body_set_shape(const RID &p_body, int32_t p_shape_idx, const RID &p_shape) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	Box2DShape2D *shape = shape_owner.get_or_null(p_shape);
	ERR_FAIL_NULL(shape);

	body->set_shape(p_shape_idx, shape);
}

void Box2DPhysicsServer2D::_body_set_shape_transform(const RID &p_body, int32_t p_shape_idx, const Transform2D &p_transform) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_shape_transform(p_shape_idx, p_transform);
}

int32_t Box2DPhysicsServer2D::_body_get_shape_count(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0);
	return body->get_shape_count();
}

RID Box2DPhysicsServer2D::_body_get_shape(const RID &p_body, int32_t p_shape_idx) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, RID());
	return body->get_shape_rid(p_shape_idx);
}

Transform2D Box2DPhysicsServer2D::_body_get_shape_transform(const RID &p_body, int32_t p_shape_idx) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, Transform2D());
	return body->get_shape_transform(p_shape_idx);
}

void Box2DPhysicsServer2D::_body_set_shape_disabled(const RID &p_body, int32_t p_shape_idx, bool p_disabled) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_shape_disabled(p_shape_idx, p_disabled);
}

void Box2DPhysicsServer2D::_body_set_shape_as_one_way_collision(const RID &p_body, int32_t p_shape_idx, bool p_enable, float p_margin) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_shape_one_way_collision(p_shape_idx, p_enable, p_margin);
}

void Box2DPhysicsServer2D::_body_remove_shape(const RID &p_body, int32_t p_shape_idx) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->remove_shape(p_shape_idx);
}

void Box2DPhysicsServer2D::_body_clear_shapes(const RID &p_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->clear_shapes();
}

void Box2DPhysicsServer2D::_body_attach_object_instance_id(const RID &p_body, uint64_t p_id) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_body_get_object_instance_id(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, ObjectID());
	return body->get_instance_id();
}

void Box2DPhysicsServer2D::_body_attach_canvas_instance_id(const RID &p_body, uint64_t p_id) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_canvas_instance_id(ObjectID(p_id));
}

uint64_t Box2DPhysicsServer2D::_body_get_canvas_instance_id(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, ObjectID());
	return body->get_canvas_instance_id();
}

void Box2DPhysicsServer2D::_body_set_continuous_collision_detection_mode(const RID &p_body, PhysicsServer2D::CCDMode p_mode) {
	if (p_mode == CCDMode::CCD_MODE_CAST_RAY) {
		WARN_PRINT_ONCE("Box2D: 'Cast Ray' CCD mode is not supported.");
		return;
	}

	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	body->set_bullet(p_mode == CCDMode::CCD_MODE_CAST_SHAPE);
}

PhysicsServer2D::CCDMode Box2DPhysicsServer2D::_body_get_continuous_collision_detection_mode(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, CCDMode::CCD_MODE_DISABLED);
	return body->get_bullet() ? CCDMode::CCD_MODE_DISABLED : CCDMode::CCD_MODE_CAST_SHAPE;
}

void Box2DPhysicsServer2D::_body_set_collision_layer(const RID &p_body, uint32_t p_layer) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_collision_layer(p_layer);
}

uint32_t Box2DPhysicsServer2D::_body_get_collision_layer(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0);
	return body->get_collision_layer();
}

void Box2DPhysicsServer2D::_body_set_collision_mask(const RID &p_body, uint32_t p_mask) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_collision_mask(p_mask);
}

uint32_t Box2DPhysicsServer2D::_body_get_collision_mask(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0);
	return body->get_collision_mask();
}

void Box2DPhysicsServer2D::_body_set_collision_priority(const RID &p_body, float p_priority) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_character_collision_priority(p_priority);
}

float Box2DPhysicsServer2D::_body_get_collision_priority(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0.0);
	return body->get_character_collision_priority();
}

void Box2DPhysicsServer2D::_body_set_param(const RID &p_body, PhysicsServer2D::BodyParameter p_param, const Variant &p_value) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	switch (p_param) {
		case BodyParameter::BODY_PARAM_BOUNCE:
			body->set_bounce(p_value);
			break;
		case BodyParameter::BODY_PARAM_FRICTION:
			body->set_friction(p_value);
			break;
		case BodyParameter::BODY_PARAM_MASS:
			body->set_mass(p_value);
			break;
		case BodyParameter::BODY_PARAM_INERTIA:
			body->set_inertia(p_value);
			break;
		case BodyParameter::BODY_PARAM_CENTER_OF_MASS:
			body->set_center_of_mass(p_value);
			break;
		case BodyParameter::BODY_PARAM_GRAVITY_SCALE:
			body->set_gravity_scale(p_value);
			break;
		case BodyParameter::BODY_PARAM_LINEAR_DAMP_MODE:
			body->set_linear_damp_mode((BodyDampMode)(int)p_value);
			break;
		case BodyParameter::BODY_PARAM_ANGULAR_DAMP_MODE:
			body->set_angular_damp_mode((BodyDampMode)(int)p_value);
			break;
		case BodyParameter::BODY_PARAM_LINEAR_DAMP:
			body->set_linear_damping(p_value);
			break;
		case BodyParameter::BODY_PARAM_ANGULAR_DAMP:
			body->set_angular_damping(p_value);
			break;
		default:
			break;
	}
}

Variant Box2DPhysicsServer2D::_body_get_param(const RID &p_body, PhysicsServer2D::BodyParameter p_param) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, Variant());

	switch (p_param) {
		case BodyParameter::BODY_PARAM_BOUNCE:
			return body->get_bounce();
		case BodyParameter::BODY_PARAM_FRICTION:
			return body->get_friction();
		case BodyParameter::BODY_PARAM_MASS:
			return body->get_mass();
		case BodyParameter::BODY_PARAM_INERTIA:
			return body->get_inertia();
		case BodyParameter::BODY_PARAM_CENTER_OF_MASS:
			return body->get_center_of_mass();
		case BodyParameter::BODY_PARAM_GRAVITY_SCALE:
			return body->get_gravity_scale();
		case BodyParameter::BODY_PARAM_LINEAR_DAMP_MODE:
			return body->get_linear_damp_mode();
		case BodyParameter::BODY_PARAM_ANGULAR_DAMP_MODE:
			return body->get_angular_damp_mode();
		case BodyParameter::BODY_PARAM_LINEAR_DAMP:
			return body->get_linear_damping();
		case BodyParameter::BODY_PARAM_ANGULAR_DAMP:
			return body->get_angular_damping();
		default:
			return Variant();
	}
}

void Box2DPhysicsServer2D::_body_reset_mass_properties(const RID &p_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
}

void Box2DPhysicsServer2D::_body_set_state(const RID &p_body, PhysicsServer2D::BodyState p_state, const Variant &p_value) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);

	switch (p_state) {
		case BodyState::BODY_STATE_TRANSFORM:
			body->set_transform(p_value, true);
			break;
		case BodyState::BODY_STATE_LINEAR_VELOCITY:
			body->set_linear_velocity(p_value);
			break;
		case BodyState::BODY_STATE_ANGULAR_VELOCITY:
			body->set_angular_velocity(p_value);
			break;
		case BodyState::BODY_STATE_SLEEPING:
			body->set_sleep_state(p_value);
			break;
		case BodyState::BODY_STATE_CAN_SLEEP:
			body->set_sleep_enabled(p_value);
			break;
		default:
			break;
	}
}

Variant Box2DPhysicsServer2D::_body_get_state(const RID &p_body, PhysicsServer2D::BodyState p_state) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, Variant());

	switch (p_state) {
		case BodyState::BODY_STATE_TRANSFORM:
			return body->get_transform();
		case BodyState::BODY_STATE_LINEAR_VELOCITY:
			return body->get_linear_velocity();
		case BodyState::BODY_STATE_ANGULAR_VELOCITY:
			return body->get_angular_velocity();
		case BodyState::BODY_STATE_SLEEPING:
			return body->is_sleeping();
		case BodyState::BODY_STATE_CAN_SLEEP:
			return body->can_sleep();
		default:
			return Variant();
	}
}

void Box2DPhysicsServer2D::_body_apply_central_impulse(const RID &p_body, const Vector2 &p_impulse) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_impulse_center(p_impulse);
}

void Box2DPhysicsServer2D::_body_apply_torque_impulse(const RID &p_body, float p_impulse) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_torque_impulse(p_impulse);
}

void Box2DPhysicsServer2D::_body_apply_impulse(const RID &p_body, const Vector2 &p_impulse, const Vector2 &p_position) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_impulse(p_impulse, p_position);
}

void Box2DPhysicsServer2D::_body_apply_central_force(const RID &p_body, const Vector2 &p_force) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_central_force(p_force);
}

void Box2DPhysicsServer2D::_body_apply_force(const RID &p_body, const Vector2 &p_force, const Vector2 &p_position) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_force(p_force, p_position);
}

void Box2DPhysicsServer2D::_body_apply_torque(const RID &p_body, float p_torque) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->apply_torque(p_torque);
}

void Box2DPhysicsServer2D::_body_add_constant_central_force(const RID &p_body, const Vector2 &p_force) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->add_constant_central_force(p_force);
}

void Box2DPhysicsServer2D::_body_add_constant_force(const RID &p_body, const Vector2 &p_force, const Vector2 &p_position) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->add_constant_force(p_force, p_position);
}

void Box2DPhysicsServer2D::_body_add_constant_torque(const RID &p_body, float p_torque) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->add_constant_torque(p_torque);
}

void Box2DPhysicsServer2D::_body_set_constant_force(const RID &p_body, const Vector2 &p_force) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_constant_force(p_force);
}

Vector2 Box2DPhysicsServer2D::_body_get_constant_force(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, Vector2());
	return body->get_constant_force();
}

void Box2DPhysicsServer2D::_body_set_constant_torque(const RID &p_body, float p_torque) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_constant_torque(p_torque);
}

float Box2DPhysicsServer2D::_body_get_constant_torque(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0.0);
	return body->get_constant_torque();
}

void Box2DPhysicsServer2D::_body_set_axis_velocity(const RID &p_body, const Vector2 &p_axis_velocity) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	Vector2 axis = p_axis_velocity.normalized();
	Vector2 linear_velocity = body->get_linear_velocity();
	linear_velocity -= axis * axis.dot(linear_velocity);
	linear_velocity += p_axis_velocity;
	body->set_linear_velocity(linear_velocity);
}

void Box2DPhysicsServer2D::_body_add_collision_exception(const RID &p_body, const RID &p_excepted_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->add_collision_exception(p_excepted_body);
}

void Box2DPhysicsServer2D::_body_remove_collision_exception(const RID &p_body, const RID &p_excepted_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->remove_collision_exception(p_excepted_body);
}

TypedArray<RID> Box2DPhysicsServer2D::_body_get_collision_exceptions(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, {});
	return body->get_collision_exceptions();
}

void Box2DPhysicsServer2D::_body_set_max_contacts_reported(const RID &p_body, int32_t p_amount) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_max_contacts_reported(p_amount);
}

int32_t Box2DPhysicsServer2D::_body_get_max_contacts_reported(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0);
	return body->get_max_contacts_reported();
}

void Box2DPhysicsServer2D::_body_set_contacts_reported_depth_threshold(const RID &p_body, float p_threshold) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	body->set_contact_depth_threshold(p_threshold);
}

float Box2DPhysicsServer2D::_body_get_contacts_reported_depth_threshold(const RID &p_body) const {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, 0.0);
	return body->get_contact_depth_threshold();
}

void Box2DPhysicsServer2D::_body_set_omit_force_integration(const RID &p_body, bool p_enable) {
	ERR_PRINT_ONCE("Box2D: Custom Integrator / Omit Force Integration is not supported. Any values set will be ignored.");
}

bool Box2DPhysicsServer2D::_body_is_omitting_force_integration(const RID &p_body) const {
	WARN_PRINT_ONCE("Box2D: Custom Integrator / Omit Force Integration is not supported.");
	return false;
}

void Box2DPhysicsServer2D::_body_set_state_sync_callback(const RID &p_body, const Callable &p_callable) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_state_sync_callback(p_callable);
}

void Box2DPhysicsServer2D::_body_set_force_integration_callback(const RID &p_body, const Callable &p_callable, const Variant &p_userdata) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL(body);
	return body->set_force_integration_callback(p_callable, p_userdata);
}

PhysicsDirectBodyState2D *Box2DPhysicsServer2D::_body_get_direct_state(const RID &p_body) {
	Box2DBody2D *body = body_owner.get_or_null(p_body);
	ERR_FAIL_NULL_V(body, nullptr);
	return body->get_direct_state();
}

bool Box2DPhysicsServer2D::_body_test_motion(
		const RID &p_body,
		const Transform2D &p_from,
		const Vector2 &p_motion,
		float p_margin,
		bool p_collide_separation_ray,
		bool p_recovery_as_collision,
		PhysicsServer2DExtensionMotionResult *p_result) const {
	return false;
}

// Joint API
RID Box2DPhysicsServer2D::_joint_create() {
	Box2DJoint2D *joint = memnew(Box2DJoint2D);
	RID joint_rid = joint_owner.make_rid(joint);
	joint->set_rid(joint_rid);
	return joint_rid;
}

void Box2DPhysicsServer2D::_joint_clear(const RID &p_joint) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);

	if (joint->get_type() != JOINT_TYPE_MAX) {
		Box2DJoint2D *empty_joint = memnew(Box2DJoint2D);
		empty_joint->set_rid(joint->get_rid());

		memdelete(joint);
		joint_owner.replace(p_joint, empty_joint);
	}
}

void Box2DPhysicsServer2D::_joint_set_param(const RID &p_joint, PhysicsServer2D::JointParam p_param, float p_value) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);

	switch (p_param) {
		case JointParam::JOINT_PARAM_BIAS:
			joint->set_bias(p_value);
			break;
		case JointParam::JOINT_PARAM_MAX_BIAS:
			joint->set_max_bias(p_value);
			break;
		case JointParam::JOINT_PARAM_MAX_FORCE:
			joint->set_max_force(p_value);
			break;
		default:
			break;
	}
}

float Box2DPhysicsServer2D::_joint_get_param(const RID &p_joint, PhysicsServer2D::JointParam p_param) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, 0.0);

	switch (p_param) {
		case JointParam::JOINT_PARAM_BIAS:
			return joint->get_bias();
		case JointParam::JOINT_PARAM_MAX_BIAS:
			return joint->get_max_bias();
		case JointParam::JOINT_PARAM_MAX_FORCE:
			return joint->get_max_force();
		default:
			ERR_FAIL_V(0.0);
	}
}

void Box2DPhysicsServer2D::_joint_disable_collisions_between_bodies(const RID &p_joint, bool p_disable) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);
	joint->disable_collisions_between_bodies(p_disable);
}

bool Box2DPhysicsServer2D::_joint_is_disabled_collisions_between_bodies(const RID &p_joint) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, false);
	return joint->is_disabled_collisions_between_bodies();
}

void Box2DPhysicsServer2D::_joint_make_pin(const RID &p_joint, const Vector2 &p_anchor, const RID &p_body_a, const RID &p_body_b) {
	Box2DJoint2D *old_joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(old_joint);

	Box2DBody2D *body_a = body_owner.get_or_null(p_body_a);
	ERR_FAIL_NULL(body_a);

	Box2DBody2D *body_b = body_owner.get_or_null(p_body_b);
	ERR_FAIL_NULL(body_b);
	ERR_FAIL_COND(body_a == body_b);

	Box2DJoint2D *new_joint = memnew(Box2DPinJoint2D(p_anchor, body_a, body_b));
	new_joint->copy_settings_from(old_joint);
	joint_owner.replace(p_joint, new_joint);

	memdelete(old_joint);
}

void Box2DPhysicsServer2D::_joint_make_groove(const RID &p_joint, const Vector2 &p_a_groove1, const Vector2 &p_a_groove2, const Vector2 &p_b_anchor, const RID &p_body_a, const RID &p_body_b) {
	Box2DJoint2D *old_joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(old_joint);

	Box2DBody2D *body_a = body_owner.get_or_null(p_body_a);
	ERR_FAIL_NULL(body_a);

	Box2DBody2D *body_b = body_owner.get_or_null(p_body_b);
	ERR_FAIL_NULL(body_b);
	ERR_FAIL_COND(body_a == body_b);

	Box2DJoint2D *new_joint = memnew(Box2DGrooveJoint2D(p_a_groove1, p_a_groove2, p_b_anchor, body_a, body_b));
	new_joint->copy_settings_from(old_joint);
	joint_owner.replace(p_joint, new_joint);

	memdelete(old_joint);
}

void Box2DPhysicsServer2D::_joint_make_damped_spring(const RID &p_joint, const Vector2 &p_anchor_a, const Vector2 &p_anchor_b, const RID &p_body_a, const RID &p_body_b) {
	Box2DJoint2D *old_joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(old_joint);

	Box2DBody2D *body_a = body_owner.get_or_null(p_body_a);
	ERR_FAIL_NULL(body_a);

	Box2DBody2D *body_b = body_owner.get_or_null(p_body_b);
	ERR_FAIL_NULL(body_b);
	ERR_FAIL_COND(body_a == body_b);

	Box2DJoint2D *new_joint = memnew(Box2DDampedSpringJoint2D(p_anchor_a, p_anchor_b, body_a, body_b));
	new_joint->copy_settings_from(old_joint);
	joint_owner.replace(p_joint, new_joint);

	memdelete(old_joint);
}

void Box2DPhysicsServer2D::_pin_joint_set_flag(const RID &p_joint, PhysicsServer2D::PinJointFlag p_flag, bool p_enabled) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);
	ERR_FAIL_COND(joint->get_type() != JOINT_TYPE_PIN);

	Box2DPinJoint2D *pin_joint = static_cast<Box2DPinJoint2D *>(joint);
	switch (p_flag) {
		case PinJointFlag::PIN_JOINT_FLAG_ANGULAR_LIMIT_ENABLED:
			pin_joint->set_limit_enabled(p_enabled);
			break;
		case PinJointFlag::PIN_JOINT_FLAG_MOTOR_ENABLED:
			pin_joint->set_motor_enabled(p_enabled);
			break;
		default:
			break;
	}
}

bool Box2DPhysicsServer2D::_pin_joint_get_flag(const RID &p_joint, PhysicsServer2D::PinJointFlag p_flag) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, false);
	ERR_FAIL_COND_V(joint->get_type() != JOINT_TYPE_PIN, false);

	Box2DPinJoint2D *pin_joint = static_cast<Box2DPinJoint2D *>(joint);
	switch (p_flag) {
		case PinJointFlag::PIN_JOINT_FLAG_ANGULAR_LIMIT_ENABLED:
			return pin_joint->get_limit_enabled();
		case PinJointFlag::PIN_JOINT_FLAG_MOTOR_ENABLED:
			return pin_joint->get_motor_enabled();
		default:
			ERR_FAIL_V(false);
	}
}

void Box2DPhysicsServer2D::_pin_joint_set_param(const RID &p_joint, PhysicsServer2D::PinJointParam p_param, float p_value) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);
	ERR_FAIL_COND(joint->get_type() != JOINT_TYPE_PIN);

	Box2DPinJoint2D *pin_joint = static_cast<Box2DPinJoint2D *>(joint);

	switch (p_param) {
		case PinJointParam::PIN_JOINT_LIMIT_UPPER:
			pin_joint->set_upper_limit(p_value);
			break;
		case PinJointParam::PIN_JOINT_LIMIT_LOWER:
			pin_joint->set_lower_limit(p_value);
			break;
		case PinJointParam::PIN_JOINT_MOTOR_TARGET_VELOCITY:
			pin_joint->set_motor_speed(p_value);
			break;
		case PinJointParam::PIN_JOINT_SOFTNESS:
			break;
		default:
			break;
	}
}

float Box2DPhysicsServer2D::_pin_joint_get_param(const RID &p_joint, PhysicsServer2D::PinJointParam p_param) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, 0.0);
	ERR_FAIL_COND_V(joint->get_type() != JOINT_TYPE_PIN, 0.0);

	Box2DPinJoint2D *pin_joint = static_cast<Box2DPinJoint2D *>(joint);

	switch (p_param) {
		case PinJointParam::PIN_JOINT_LIMIT_UPPER:
			return pin_joint->get_upper_limit();
		case PinJointParam::PIN_JOINT_LIMIT_LOWER:
			return pin_joint->get_lower_limit();
		case PinJointParam::PIN_JOINT_MOTOR_TARGET_VELOCITY:
			return pin_joint->get_motor_speed();
		case PinJointParam::PIN_JOINT_SOFTNESS:
			return 0.0;
		default:
			ERR_FAIL_V(0.0);
	}
}

void Box2DPhysicsServer2D::_damped_spring_joint_set_param(const RID &p_joint, PhysicsServer2D::DampedSpringParam p_param, float p_value) {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL(joint);
	ERR_FAIL_COND(joint->get_type() != JOINT_TYPE_DAMPED_SPRING);

	Box2DDampedSpringJoint2D *spring_joint = static_cast<Box2DDampedSpringJoint2D *>(joint);

	switch (p_param) {
		case DampedSpringParam::DAMPED_SPRING_REST_LENGTH:
			spring_joint->set_rest_length(p_value);
			break;
		case DampedSpringParam::DAMPED_SPRING_STIFFNESS:
			spring_joint->set_stiffness(p_value);
			break;
		case DampedSpringParam::DAMPED_SPRING_DAMPING:
			spring_joint->set_damping_ratio(p_value);
			break;
		default:
			break;
	}
}

float Box2DPhysicsServer2D::_damped_spring_joint_get_param(const RID &p_joint, PhysicsServer2D::DampedSpringParam p_param) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, 0.0);
	ERR_FAIL_COND_V(joint->get_type() != JOINT_TYPE_DAMPED_SPRING, 0.0);

	Box2DDampedSpringJoint2D *spring_joint = static_cast<Box2DDampedSpringJoint2D *>(joint);

	switch (p_param) {
		case DampedSpringParam::DAMPED_SPRING_REST_LENGTH:
			return spring_joint->get_rest_length();
		case DampedSpringParam::DAMPED_SPRING_STIFFNESS:
			return spring_joint->get_stiffness();
		case DampedSpringParam::DAMPED_SPRING_DAMPING:
			return spring_joint->get_damping_ratio();
		default:
			ERR_FAIL_V(0.0);
	}
}

PhysicsServer2D::JointType Box2DPhysicsServer2D::_joint_get_type(const RID &p_joint) const {
	Box2DJoint2D *joint = joint_owner.get_or_null(p_joint);
	ERR_FAIL_NULL_V(joint, JointType::JOINT_TYPE_MAX);
	return joint->get_type();
}

void Box2DPhysicsServer2D::_free_rid(const RID &p_rid) {
	if (shape_owner.owns(p_rid)) {
		Box2DShape2D *shape = shape_owner.get_or_null(p_rid);
		shape_owner.free(p_rid);
		memdelete(shape);
	} else if (body_owner.owns(p_rid)) {
		Box2DBody2D *body = body_owner.get_or_null(p_rid);
		body->set_free();
		body_owner.free(p_rid);
		bodies_to_delete.push_back(body);
	} else if (area_owner.owns(p_rid)) {
		Box2DArea2D *area = area_owner.get_or_null(p_rid);
		area->set_free();
		area_owner.free(p_rid);
		areas_to_delete.push_back(area);
	} else if (space_owner.owns(p_rid)) {
		Box2DSpace2D *space = space_owner.get_or_null(p_rid);
		active_spaces.erase(space);
		space_owner.free(p_rid);
		memdelete(space);
	} else if (joint_owner.owns(p_rid)) {
		Box2DJoint2D *joint = joint_owner.get_or_null(p_rid);
		joint_owner.free(p_rid);
		memdelete(joint);
	} else {
		ERR_FAIL_MSG("Attempted to free invalid RID.");
	}
}

void Box2DPhysicsServer2D::_set_active(bool p_active) {
	active = p_active;
}

void Box2DPhysicsServer2D::_init() {
	box2d_set_pixels_per_meter(Box2DProjectSettings::get_pixels_per_meter());
}

void Box2DPhysicsServer2D::_step(float p_step) {
	TracyZoneScoped("Step");

	if (active) {
		for (Box2DSpace2D *active_space : active_spaces) {
			active_space->step((float)p_step);
		}
	}

	for (Box2DArea2D *p_area : areas_to_delete) {
		memdelete(p_area);
	}
	areas_to_delete.clear();

	for (Box2DBody2D *p_body : bodies_to_delete) {
		memdelete(p_body);
	}
	bodies_to_delete.clear();

#ifdef TRACY_ENABLE
	FrameMark;
#endif
}

void Box2DPhysicsServer2D::_flush_queries() {
	TracyZoneScoped("Flush Queries");

	if (!active) {
		return;
	}

	flushing_queries = true;

	for (Box2DSpace2D *space : active_spaces) {
		space->sync_state();
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
