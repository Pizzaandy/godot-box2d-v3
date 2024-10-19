#pragma once

#include "../bodies/chain_segment_range.h"
#include "box2d_shape_2d.h"

class Box2DShape2D;

struct ShapeID {
	enum Type {
		DEFAULT,
		CHAIN,
	};

	Type type = Type::DEFAULT;
	b2ChainId chain_id = b2_nullChainId;
	b2ShapeId shape_id = b2_nullShapeId;

	ShapeID() = default;

	ShapeID(b2ShapeId p_shape_id) {
		shape_id = p_shape_id;
	}

	ShapeID(b2ChainId p_chain_id) {
		type = Type::CHAIN;
		chain_id = p_chain_id;
	}

	bool is_valid() {
		if (type == Type::DEFAULT) {
			return B2_IS_NON_NULL(shape_id);
		} else {
			return B2_IS_NON_NULL(chain_id);
		}
	}
};

class Box2DShapeInstance {
public:
	int index = -1;
	ShapeID shape_id;
	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	real_t one_way_collision_margin = 0.0;

	void assign_shape(Box2DShape2D *p_shape);

	Box2DShape2D *get_shape_or_null() { return shape; }

	void build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def);

	void destroy();

	~Box2DShapeInstance() {
		destroy();
		assign_shape(nullptr);
	}

private:
	Box2DShape2D *shape = nullptr;
};