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