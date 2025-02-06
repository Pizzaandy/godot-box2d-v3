#include "box2d_shape_instance.h"
#include "../bodies/box2d_collision_object_2d.h"

Box2DShapeInstance::Box2DShapeInstance(Box2DCollisionObject2D *p_body,
		Box2DShape2D *p_shape,
		const Transform2D &p_transform,
		bool p_disabled) :
		body(p_body),
		shape(p_shape),
		transform(p_transform),
		disabled(p_disabled) {
	if (shape) {
		shape->add_body_reference(body);
	}
}

Box2DShapeInstance::~Box2DShapeInstance() {
	if (shape) {
		return;
	}
	shape->remove_body_reference(body);
	shape->remove_from_body(body->get_body_id(), this);
}

void Box2DShapeInstance::build() {
	ERR_FAIL_COND(!shape);
	ERR_FAIL_COND(!body);

	shape->remove_from_body(body->get_body_id(), this);

	if (disabled) {
		return;
	}

	shape->add_to_body(body->get_body_id(), this);
}

Transform2D Box2DShapeInstance::get_shape_transform() const {
	Transform2D parent_transform = body->get_transform();
	Transform2D parent_scale_and_skew = Transform2D(0.0, parent_transform.get_scale(), parent_transform.get_skew(), Vector2());
	return parent_transform * transform;
}