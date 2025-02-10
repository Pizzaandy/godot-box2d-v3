#include "box2d_joint_2d.h"

Box2DJoint2D::~Box2DJoint2D() {
	destroy_joint();
}

void Box2DJoint2D::copy_settings_from(Box2DJoint2D *p_joint) {
	set_rid(p_joint->get_rid());
	set_max_force(p_joint->get_max_force());
	set_bias(p_joint->get_bias());
	set_max_bias(p_joint->get_max_bias());
}

void Box2DJoint2D::destroy_joint() {
	if (b2Joint_IsValid(joint_id)) {
		b2DestroyJoint(joint_id);
	}
	joint_id = b2_nullJointId;
}

void Box2DJoint2D::disable_collisions_between_bodies(bool p_disabled) {
	disabled_collisions_between_bodies = p_disabled;

	if (b2Joint_IsValid(joint_id)) {
		b2Joint_SetCollideConnected(joint_id, !disabled_collisions_between_bodies);
	}
}