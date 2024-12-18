#include "box2d_damped_spring_joint_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DDampedSpringJoint2D::Box2DDampedSpringJoint2D(const Vector2 &p_anchor_a, const Vector2 &p_anchor_b, Box2DBody2D *p_body_a, Box2DBody2D *p_body_b) :
		Box2DJoint2D(p_body_a, p_body_b) {
	if (!p_body_a || !p_body_b) {
		return;
	}

	ERR_FAIL_COND(!p_body_a->get_space());
	ERR_FAIL_COND(p_body_a->get_space() != p_body_b->get_space());
	space = p_body_a->get_space();

	Vector2 anchor_a = p_body_a->get_transform().affine_inverse().xform(p_anchor_a);
	Vector2 anchor_b = p_body_b->get_transform().affine_inverse().xform(p_anchor_b);

	distance_def.bodyIdA = p_body_a->get_body_id();
	distance_def.bodyIdB = p_body_b->get_body_id();
	distance_def.localAnchorA = to_box2d(anchor_a);
	distance_def.localAnchorB = to_box2d(anchor_b);

	distance_def.enableSpring = true;
	distance_def.length = 0.0;
	distance_def.dampingRatio = 1.5;
	distance_def.hertz = 20.0;

	distance_def.collideConnected = !disabled_collisions_between_bodies;

	joint_id = b2CreateDistanceJoint(space->get_world_id(), &distance_def);
}

void Box2DDampedSpringJoint2D::set_rest_length(float p_length) {
	distance_def.length = to_box2d(p_length);
	if (b2Joint_IsValid(joint_id)) {
		b2DistanceJoint_SetLength(joint_id, distance_def.length);
	}
}

void Box2DDampedSpringJoint2D::set_damping_ratio(float p_damping) {
	distance_def.dampingRatio = p_damping;
	if (b2Joint_IsValid(joint_id)) {
		b2DistanceJoint_SetSpringDampingRatio(joint_id, distance_def.dampingRatio);
	}
}

void Box2DDampedSpringJoint2D::set_stiffness(float p_hertz) {
	distance_def.hertz = p_hertz;
	if (b2Joint_IsValid(joint_id)) {
		b2DistanceJoint_SetSpringHertz(joint_id, distance_def.hertz);
	}
}