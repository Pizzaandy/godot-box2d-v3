#pragma once

#include "../bodies/chain_segment_range.h"
#include "box2d_shape_2d.h"

class Box2DShape2D;

struct ShapeID {
	enum Type {
		INVALID,
		DEFAULT,
		CHAIN,
	};

	Type type = Type::INVALID;
	b2ChainId chain_id = b2_nullChainId;
	b2ShapeId shape_id = b2_nullShapeId;

	ShapeID() = default;

	ShapeID(b2ShapeId p_shape_id) {
		type = Type::DEFAULT;
		shape_id = p_shape_id;
	}

	ShapeID(b2ChainId p_chain_id) {
		type = Type::CHAIN;
		chain_id = p_chain_id;
	}

	static ShapeID invalid() {
		return {};
	}
};

class Box2DShapeInstance {
public:
	int index = -1;
	ShapeID shape_id;
	Transform2D transform;
	bool disabled = false;
	bool one_way_collision = false;
	float one_way_collision_margin = 0.0;

	void set_shape(Box2DShape2D *p_shape);

	Box2DShape2D *get_shape_or_null() const { return shape; }

	void build(b2BodyId p_body, const Transform2D &p_transform, const b2ShapeDef &p_shape_def);

	void destroy();

	~Box2DShapeInstance() {
		destroy();
		set_shape(nullptr);
	}

private:
	Box2DShape2D *shape = nullptr;
};