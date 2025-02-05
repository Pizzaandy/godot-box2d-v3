#include "box2d_shape_instance.h"
#include "../bodies/box2d_collision_object_2d.h"

void Box2DShapeInstance::set_shape(Box2DShape2D *p_shape) {
	if (shape) {
		shape->remove_instance(this);
	}

	if (!p_shape) {
		shape = nullptr;
		return;
	}

	shape = p_shape;
	p_shape->add_instance(this);
}

void Box2DShapeInstance::build() {
	ERR_FAIL_COND(!shape);
	ERR_FAIL_COND(!body);

	shape->remove_from_body(body, this);

	if (disabled) {
		return;
	}

	Transform2D parent_transform = body->get_transform();
	Transform2D parent_scale_and_skew = Transform2D(0.0, parent_transform.get_scale(), parent_transform.get_skew(), Vector2());

	shape->add_to_body(body, parent_scale_and_skew * transform, body->get_shape_def());
}