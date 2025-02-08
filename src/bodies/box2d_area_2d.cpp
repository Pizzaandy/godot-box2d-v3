#include "box2d_area_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DArea2D::Box2DArea2D() :
		Box2DCollisionObject2D(Type::AREA) {
	body_def.type = b2_kinematicBody;
	mode = PhysicsServer2D::BODY_MODE_KINEMATIC;
	shape_def.isSensor = true;
}

void Box2DArea2D::step() {
	// Apply overrides
}

void Box2DArea2D::report_event(
		Type p_type,
		PhysicsServer2D::AreaBodyStatus p_status,
		const RID &p_other_rid,
		ObjectID p_other_instance_id,
		int32_t p_other_shape_index,
		int32_t p_self_shape_index) const {
	static thread_local Array arguments = []() {
		Array array;
		array.resize(5);
		return array;
	}();

	arguments[0] = p_status;
	arguments[1] = p_other_rid;
	arguments[2] = p_other_instance_id;
	arguments[3] = p_other_shape_index;
	arguments[4] = p_self_shape_index;

	if (p_type == Type::AREA) {
		ERR_FAIL_COND(!area_monitor_callback.is_valid());
		area_monitor_callback.callv(arguments);
	} else {
		ERR_FAIL_COND(!body_monitor_callback.is_valid());
		body_monitor_callback.callv(arguments);
	}
}

bool Box2DArea2D::is_default_area() const {
	return space && space->get_default_area() == this;
}

void Box2DArea2D::set_gravity_strength(float p_strength) {
	gravity_strength = p_strength;
	gravity_changed();
}

void Box2DArea2D::set_gravity_direction(Vector2 p_direction) {
	gravity_direction = p_direction.normalized();
	gravity_changed();
}

void Box2DArea2D::gravity_changed() {
	if (is_default_area()) {
		space->set_default_gravity(gravity_direction * gravity_strength);
	}
}

void Box2DArea2D::set_monitorable(float p_monitorable) {
	monitorable = p_monitorable;

	set_collision_mask(shape_def.filter.maskBits);
}

uint64_t Box2DArea2D::modify_mask_bits(uint32_t p_mask) {
	return monitorable ? p_mask | AREA_MONITORABLE_MASK_BIT : p_mask;
}

uint64_t Box2DArea2D::modify_layer_bits(uint32_t p_layer) {
	return p_layer | AREA_MONITORABLE_MASK_BIT;
}