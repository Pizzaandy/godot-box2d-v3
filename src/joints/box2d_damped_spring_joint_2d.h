#pragma once

#include "box2d_joint_2d.h"

class Box2DDampedSpringJoint2D : public Box2DJoint2D {
public:
	Box2DDampedSpringJoint2D(const Vector2 &p_anchor_a, const Vector2 &p_anchor_b, Box2DBody2D *p_body_a, Box2DBody2D *p_body_b);
	PhysicsServer2D::JointType get_type() const override { return PhysicsServer2D::JOINT_TYPE_DAMPED_SPRING; }

	void set_rest_length(float p_length);
	float get_rest_length() const { return distance_def.length; }

	void set_damping_ratio(float p_damping);
	float get_damping_ratio() const { return distance_def.dampingRatio; }

	void set_stiffness(float p_hertz);
	float get_stiffness() const { return distance_def.hertz; }

private:
	b2DistanceJointDef distance_def = b2DefaultDistanceJointDef();
};