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

void Box2DShapeInstance::build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def) {
	destroy();

	if (disabled) {
		return;
	}

	ERR_FAIL_COND(!shape);
	shape_id = shape->build(p_body, p_transform, p_shape_def);

	if (shape_id.type == ShapeID::CHAIN) {
		ChainSegmentRange range(shape_id.chain_id);
		for (b2ShapeId id : range) {
			b2Shape_SetUserData(id, this);
		}
	} else {
		b2Shape_SetUserData(shape_id.shape_id, this);
	}
}

void Box2DShapeInstance::destroy() {
	if (b2Shape_IsValid(shape_id.shape_id)) {
		b2DestroyShape(shape_id.shape_id, false);
	} else if (b2Chain_IsValid(shape_id.chain_id)) {
		b2DestroyChain(shape_id.chain_id);
	}
	shape_id = {};
}