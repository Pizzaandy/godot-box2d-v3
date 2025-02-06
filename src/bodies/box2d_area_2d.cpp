#include "box2d_area_2d.h"
#include "../spaces/box2d_space_2d.h"

Box2DArea2D::Box2DArea2D() :
		Box2DCollisionObject2D(Type::AREA) {
	body_def.type = b2_kinematicBody;
	mode = PhysicsServer2D::BODY_MODE_KINEMATIC;
	shape_def.isSensor = true;
	shape_def.filter.categoryBits = 1;
	shape_def.filter.maskBits = 1 | AREA_MASK_BIT;
}

void Box2DArea2D::step() {
	// Apply overrides
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