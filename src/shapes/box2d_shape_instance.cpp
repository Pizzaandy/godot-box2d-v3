#include "box2d_shape_instance.h"
#include "../bodies/box2d_collision_object_2d.h"

Box2DShapeInstance::Box2DShapeInstance(
		Box2DCollisionObject2D *p_object,
		Box2DShape2D *p_shape,
		const Transform2D &p_transform,
		bool p_disabled) :
		object(p_object),
		shape(p_shape),
		transform(p_transform),
		disabled(p_disabled) {
	ERR_FAIL_NULL(shape);
	shape->add_object_reference(object);
	shape_type = shape->get_type();
}

Box2DShapeInstance::~Box2DShapeInstance() {
	if (!shape || !object) {
		return;
	}
	shape->remove_from_body(object->get_body_id(), this);
	shape->remove_object_reference(object);
}

void Box2DShapeInstance::set_shape(Box2DShape2D *p_shape) {
	shape = p_shape;
	shape_type = shape->get_type();
}

void Box2DShapeInstance::build() {
	ERR_FAIL_COND(!shape);
	ERR_FAIL_COND(!object);

	shape->remove_from_body(object->get_body_id(), this);

	if (disabled) {
		return;
	}

	shape->add_to_body(object->get_body_id(), this);
}

Transform2D Box2DShapeInstance::get_shape_transform() const {
	Transform2D parent_transform = object->get_transform();
	Transform2D parent_scale_and_skew = Transform2D(0.0, parent_transform.get_scale(), parent_transform.get_skew(), Vector2());
	return parent_scale_and_skew * transform;
}

b2ShapeDef Box2DShapeInstance::get_shape_def() {
	b2ShapeDef shape_def = object->get_shape_def();
	shape_def.userData = this;
	return shape_def;
}