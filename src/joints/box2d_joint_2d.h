#pragma once

#include "../bodies/box2d_body_2d.h"

class Box2DJoint2D {
public:
	Box2DJoint2D() = default;
	Box2DJoint2D(Box2DBody2D *p_body_a, Box2DBody2D *p_body_b);
	~Box2DJoint2D();

	virtual PhysicsServer2D::JointType get_type() const { return PhysicsServer2D::JOINT_TYPE_MAX; }

	void copy_settings_from(Box2DJoint2D *p_joint);

	void destroy_joint();

	void set_rid(const RID &p_rid) { rid = p_rid; }
	RID get_rid() const { return rid; }

	void disable_collisions_between_bodies(bool p_disabled);
	bool is_disabled_collisions_between_bodies() const { return disabled_collisions_between_bodies; }

	void set_max_force(float p_force) { max_force = p_force; }
	float get_max_force() const { return max_force; }

	void set_bias(float p_bias) { bias = p_bias; }
	float get_bias() const { return bias; }

	void set_max_bias(float p_bias) { max_bias = p_bias; }
	float get_max_bias() const { return max_bias; }

protected:
	Box2DBody2D *body_a;
	Box2DBody2D *body_b;

	Box2DSpace2D *space = nullptr;
	b2JointId joint_id = b2_nullJointId;

	float bias = 0;
	float max_bias = 3.40282e+38;
	float max_force = 3.40282e+38;

	bool disabled_collisions_between_bodies = true;
	RID rid;
};