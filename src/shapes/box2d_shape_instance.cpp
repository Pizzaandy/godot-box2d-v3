#include "box2d_shape_instance.h"

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

void Box2DShapeInstance::build(b2BodyId p_body, const Transform2D &p_local_transform, b2ShapeDef p_shape_def) {
	destroy();

	if (disabled) {
		return;
	}

	ERR_FAIL_COND(!shape);

	p_shape_def.userData = this;
	ShapeIdAndGeometry result = shape->add_to_body(p_body, p_local_transform, p_shape_def);

	shape_id = result.id;
	shape_info = result.info;
}

void Box2DShapeInstance::destroy() {
	if (shape_id.type == ShapeID::Type::CHAIN) {
		if (b2Chain_IsValid(shape_id.chain_id)) {
			b2DestroyChain(shape_id.chain_id);
		}
	} else if (shape_id.type == ShapeID::Type::DEFAULT) {
		if (b2Shape_IsValid(shape_id.shape_id)) {
			b2DestroyShape(shape_id.shape_id, false);
		}
	}

	shape_id = ShapeID::invalid();
}