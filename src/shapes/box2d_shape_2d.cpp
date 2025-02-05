#include "box2d_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

Box2DShape2D::~Box2DShape2D() {
}

void Box2DShape2D::set_data(const Variant &p_data) {
	data = p_data;

	if (!validate_data(p_data)) {
		return;
	}

	update_instances();
}

void Box2DShape2D::update_instances() {
	for (Box2DShapeInstance *instance : instances) {
		instance->build();
	}
}