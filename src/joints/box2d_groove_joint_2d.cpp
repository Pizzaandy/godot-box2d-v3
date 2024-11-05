#include "box2d_groove_joint_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DGrooveJoint2D::Box2DGrooveJoint2D(const Vector2 &p_a_groove1, const Vector2 &p_a_groove2, const Vector2 &p_b_anchor, Box2DBody2D *p_body_a, Box2DBody2D *p_body_b) :
		Box2DJoint2D(p_body_a, p_body_b) {
	if (!p_body_a || !p_body_b) {
		return;
	}

	ERR_FAIL_COND(!p_body_a->get_space());
	ERR_FAIL_COND(p_body_a->get_space() != p_body_b->get_space());
	space = p_body_a->get_space();

	Vector2 point_a_1 = p_body_a->get_transform().affine_inverse().xform(p_a_groove1);
	Vector2 point_a_2 = p_body_a->get_transform().affine_inverse().xform(p_a_groove2);

	Vector2 anchor_a = point_a_1;
	Vector2 anchor_b = p_body_b->get_transform().affine_inverse().xform(p_b_anchor);
	Vector2 axis = (point_a_2 - point_a_1).normalized();

	prismatic_def.bodyIdA = p_body_a->get_body_id();
	prismatic_def.bodyIdB = p_body_b->get_body_id();
	prismatic_def.localAnchorA = to_box2d(anchor_a);
	prismatic_def.localAnchorB = to_box2d(anchor_b);

	prismatic_def.enableLimit = true;
	prismatic_def.upperTranslation = to_box2d(p_a_groove1.distance_to(p_a_groove2));
	prismatic_def.lowerTranslation = 0;
	prismatic_def.localAxisA = b2Vec2{ (float)axis.x, (float)axis.y };
	prismatic_def.collideConnected = !disabled_collisions_between_bodies;

	joint_id = b2CreatePrismaticJoint(space->get_world_id(), &prismatic_def);
}