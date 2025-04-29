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
	shape->remove_from_body(this);
	shape->remove_object_reference(object);
}

void Box2DShapeInstance::set_shape(Box2DShape2D *p_shape) {
	shape = p_shape;
	shape_type = shape->get_type();
}

void Box2DShapeInstance::build() {
	ERR_FAIL_COND(!shape);
	ERR_FAIL_COND(!object);

	shape->remove_from_body(this);

	if (disabled) {
		return;
	}

	shape->add_to_body(this);
}

Transform2D Box2DShapeInstance::get_global_transform() const {
	Transform2D parent_transform = object->get_transform();
	return get_global_transform_with_parent_transform(parent_transform);
}

Transform2D Box2DShapeInstance::get_global_transform_with_parent_transform(const Transform2D &p_parent_transform) const {
	Transform2D parent_scale_and_skew = Transform2D(0.0, p_parent_transform.get_scale(), p_parent_transform.get_skew(), Vector2());
	return parent_scale_and_skew * transform;
}

b2ShapeDef Box2DShapeInstance::get_shape_def() {
	b2ShapeDef shape_def = object->get_shape_def();
	shape_def.userData = this;
	return shape_def;
}

bool Box2DShapeInstance::should_filter_one_way_collision(const Vector2 &p_motion, const Vector2 &p_normal, float p_depth) const {
	ERR_FAIL_COND_V(!shape, false);
	ERR_FAIL_COND_V(!object, false);

	if (!has_one_way_collision()) {
		return false;
	}

	Vector2 one_way_normal = -(get_transform() * get_collision_object()->get_transform()).columns[1].normalized();
	float max_allowed_depth = p_motion.length() * Math::max(p_motion.normalized().dot(one_way_normal), real_t(0.0f)) + get_one_way_collision_margin();
	if (p_normal.dot(one_way_normal) <= 0.0f || p_depth > max_allowed_depth) {
		return true;
	}

	return false;
}