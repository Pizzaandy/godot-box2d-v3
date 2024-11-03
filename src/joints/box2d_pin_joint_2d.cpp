#include "box2d_pin_joint_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DPinJoint2D::Box2DPinJoint2D(const Vector2 &p_pos, Box2DBody2D *p_body_a, Box2DBody2D *p_body_b) :
		Box2DJoint2D(p_body_a, p_body_b) {
	if (!p_body_a || !p_body_b) {
		return;
	}

	ERR_FAIL_COND(!p_body_a->get_space());
	ERR_FAIL_COND(p_body_a->get_space() != p_body_b->get_space());
	space = p_body_a->get_space();

	Vector2 anchor_a = p_body_a->get_transform().affine_inverse().xform(p_pos);
	Vector2 anchor_b = p_body_b->get_transform().affine_inverse().xform(p_pos);

	revolute_def.bodyIdA = p_body_a->get_body_id();
	revolute_def.bodyIdB = p_body_b->get_body_id();
	revolute_def.localAnchorA = to_box2d(anchor_a);
	revolute_def.localAnchorB = to_box2d(anchor_b);
	revolute_def.collideConnected = !disabled_collisions_between_bodies;

	// TODO: account for mass?
	revolute_def.maxMotorTorque = 100000.0;

	joint_id = b2CreateRevoluteJoint(space->get_world_id(), &revolute_def);
}

void Box2DPinJoint2D::set_upper_limit(float p_limit) {
	revolute_def.upperAngle = p_limit;
	if (b2Joint_IsValid(joint_id)) {
		b2RevoluteJoint_SetLimits(joint_id, revolute_def.lowerAngle, revolute_def.upperAngle);
	}
}

void Box2DPinJoint2D::set_lower_limit(float p_limit) {
	revolute_def.lowerAngle = p_limit;
	if (b2Joint_IsValid(joint_id)) {
		b2RevoluteJoint_SetLimits(joint_id, revolute_def.lowerAngle, revolute_def.upperAngle);
	}
}

void Box2DPinJoint2D::set_motor_speed(float p_speed) {
	revolute_def.motorSpeed = p_speed;
	if (b2Joint_IsValid(joint_id)) {
		b2RevoluteJoint_SetMotorSpeed(joint_id, revolute_def.motorSpeed);
	}
}

void Box2DPinJoint2D::set_motor_enabled(bool p_enabled) {
	revolute_def.enableMotor = p_enabled;
	if (b2Joint_IsValid(joint_id)) {
		b2RevoluteJoint_EnableMotor(joint_id, revolute_def.enableMotor);
	}
}

void Box2DPinJoint2D::set_limit_enabled(bool p_enabled) {
	revolute_def.enableLimit = p_enabled;
	if (b2Joint_IsValid(joint_id)) {
		b2RevoluteJoint_EnableLimit(joint_id, revolute_def.enableLimit);
	}
}