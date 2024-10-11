#include "box2d_body_2d.h"
#include <godot_cpp/variant/utility_functions.hpp>

void Box2DBody2D::set_space(Box2DSpace2D *p_space) {
	if (b2Body_IsValid(body_id)) {
		b2DestroyBody(body_id);
		body_id = b2_nullBodyId;
	}

	if (p_space != nullptr) {
		body_id = b2CreateBody(p_space->get_world_id(), &body_def);
		set_mode(mode);
	}

	space = p_space;
}

Box2DSpace2D *Box2DBody2D::get_space() {
	return space;
}

void Box2DBody2D::set_mode(PhysicsServer2D::BodyMode p_mode) {
	mode = p_mode;

	if (B2_IS_NULL(body_id)) {
		return;
	}

	switch (mode) {
		case PhysicsServer2D::BodyMode::BODY_MODE_STATIC:
			b2Body_SetType(body_id, b2BodyType::b2_staticBody);
			break;
		case PhysicsServer2D::BodyMode::BODY_MODE_KINEMATIC:
			b2Body_SetType(body_id, b2BodyType::b2_kinematicBody);
			break;
		case PhysicsServer2D::BodyMode::BODY_MODE_RIGID:
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			break;
		case PhysicsServer2D::BodyMode::BODY_MODE_RIGID_LINEAR:
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			break;
	}
}

PhysicsServer2D::BodyMode Box2DBody2D::get_mode() {
	return mode;
}

RID Box2DBody2D::get_shape_rid(int p_index) {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), RID());
	Shape shape = shapes[p_index];
	return shape.shape->get_rid();
}

void Box2DBody2D::update_shape(Shape p_shape) {
    if (B2_IS_NULL(body_id)) {
		return;
	}

    p_shape.shape_id = p_shape.shape->build(body_id, p_shape.transform, p_shape.disabled, p_shape.shape_id);
}

void Box2DBody2D::add_shape(Box2DShape2D *p_shape, Transform2D p_transform, bool p_disabled) {
	Box2DBody2D::Shape shape;
	shape.shape = p_shape;
	shape.transform = p_transform;
	shape.disabled = p_disabled;

	update_shape(shape);
	shapes.push_back(shape);
}

void Box2DBody2D::set_shape(int p_index, Box2DShape2D *p_shape) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape shape = shapes[p_index];
	shape.shape = p_shape;
	update_shape(shape);
}

void Box2DBody2D::remove_shape(int p_index) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape shape = shapes[p_index];

    if (B2_IS_NON_NULL(shape.shape_id)) {
        b2DestroyShape(shape.shape_id, true);
    }

	shapes.remove_at(p_index);
}

void Box2DBody2D::set_shape_transform(int p_index, Transform2D p_transform) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Shape shape = shapes[p_index];
	shape.transform = p_transform;
	update_shape(shape);
}

Transform2D Box2DBody2D::get_shape_transform(int p_index) {
    ERR_FAIL_INDEX_V(p_index, shapes.size(), Transform2D());
    Shape shape = shapes[p_index];
	return shape.transform;
}

int32_t Box2DBody2D::get_shape_count() {
	return shapes.size();
}
