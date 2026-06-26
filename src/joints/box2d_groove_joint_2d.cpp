#include "box2d_groove_joint_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DGrooveJoint2D::Box2DGrooveJoint2D(const Vector2 &p_a_groove1, const Vector2 &p_a_groove2, const Vector2 &p_b_anchor, Box2DBody2D *p_body_a, Box2DBody2D *p_body_b) :
		Box2DJoint2D(PhysicsServer2D::JOINT_TYPE_GROOVE, p_body_a, p_body_b) {
	if (!p_body_a || !p_body_b) {
		return;
	}

	ERR_FAIL_COND(!p_body_a->in_space());
	ERR_FAIL_COND(p_body_a->get_space() != p_body_b->get_space());
	space = p_body_a->get_space();

	Vector2 point_a_1 = p_body_a->get_transform().affine_inverse().xform(p_a_groove1);
	Vector2 point_a_2 = p_body_a->get_transform().affine_inverse().xform(p_a_groove2);

	Vector2 anchor_a = point_a_1;
	Vector2 anchor_b = p_body_b->get_transform().affine_inverse().xform(p_b_anchor);
	Vector2 axis = (point_a_2 - point_a_1).normalized();

	wheel_def.base.bodyIdA = p_body_a->get_body_id();
	wheel_def.base.bodyIdB = p_body_b->get_body_id();
	wheel_def.base.localFrameA.p = to_box2d(anchor_a);
	wheel_def.base.localFrameB.p = to_box2d(anchor_b);

	wheel_def.enableLimit = true;
	wheel_def.enableSpring = false;
	wheel_def.upperTranslation = to_box2d(p_a_groove1.distance_to(p_a_groove2));
	wheel_def.lowerTranslation = 0;
	wheel_def.base.localFrameA.q = b2MakeRot(axis.angle());

	wheel_def.base.collideConnected = !disabled_collisions_between_bodies;
	wheel_def.base.userData = this;

	joint_id = b2CreateWheelJoint(space->get_world_id(), &wheel_def);
}