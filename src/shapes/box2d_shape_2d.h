#pragma once

#include "../box2d_globals.h"
#include <godot_cpp/classes/physics_server2d.hpp>

using namespace godot;

/// @brief Builder class for shapes. Does not keep track of shape instances.
class Box2DShape2D {
public:
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

	RID get_rid() const { return rid; }
	void set_rid(const RID &p_rid) { rid = p_rid; }
	virtual ShapeID build(b2BodyId p_body, Transform2D p_transform, b2ShapeDef &p_shape_def) = 0;
	void set_data(const Variant &p_data) { data = p_data; }
	Variant get_data() const { return data; }

protected:
	RID rid;
	Variant data;
};