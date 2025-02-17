#include "box2d_collision_object_2d.h"

#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Box2DCollisionObject2D::Box2DCollisionObject2D(Type p_type) :
		type(p_type) {
	set_collision_mask(1);
	set_collision_layer(1);
}

void Box2DCollisionObject2D::set_free() {
	_is_freed = true;
	set_space(nullptr);
}

void Box2DCollisionObject2D::destroy_body() {
	if (b2Body_IsValid(body_id)) {
		b2DestroyBody(body_id);
		body_destroyed();
	}

	in_space = false;
	body_id = b2_nullBodyId;
}

void Box2DCollisionObject2D::set_space(Box2DSpace2D *p_space) {
	destroy_body();

	if (space == p_space) {
		return;
	}

	space = p_space;

	if (!space) {
		return;
	}

	ERR_FAIL_COND(space->locked);

	// Create body
	body_def.position = to_box2d(current_transform.get_origin());
	body_def.rotation = b2MakeRot(current_transform.get_rotation());

	body_def.userData = this;
	body_id = b2CreateBody(space->get_world_id(), &body_def);

	in_space = true;

	rebuild_all_shapes();

	body_created();
}

void Box2DCollisionObject2D::set_mode(PhysicsServer2D::BodyMode p_mode) {
	if (mode == p_mode) {
		return;
	}

	PhysicsServer2D::BodyMode previous_mode = mode;
	mode = p_mode;

	switch (p_mode) {
		case PhysicsServer2D::BODY_MODE_STATIC:
			body_def.type = b2_staticBody;
			if (!in_space) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_staticBody);
			break;
		case PhysicsServer2D::BODY_MODE_KINEMATIC:
			body_def.type = b2_kinematicBody;
			if (!in_space) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_kinematicBody);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID:
			body_def.type = b2_dynamicBody;
			body_def.fixedRotation = false;
			if (!in_space) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, false);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID_LINEAR:
			body_def.type = b2_dynamicBody;
			body_def.fixedRotation = true;
			if (!in_space) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, true);
			break;
		default:
			return;
	}

	shapes_changed();
}

void Box2DCollisionObject2D::set_collision_layer(uint32_t p_layer) {
	shape_def.filter.categoryBits = modify_layer_bits(p_layer);

	if (!in_space) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DCollisionObject2D::set_collision_mask(uint32_t p_mask) {
	shape_def.filter.maskBits = modify_mask_bits(p_mask) | COMMON_MASK_BIT;

	if (!in_space) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DCollisionObject2D::set_transform(const Transform2D &p_transform, bool p_move_kinematic) {
	if (!in_space) {
		current_transform = p_transform;
		return;
	}

	if (p_transform == current_transform) {
		return;
	}

	Vector2 position = p_transform.get_origin();
	float rotation = p_transform.get_rotation();
	float last_step = space->get_last_step();

	if (p_move_kinematic && mode == PhysicsServer2D::BODY_MODE_KINEMATIC) {
		if (last_step < 0.0) {
			current_transform = p_transform;
			return;
		}

		Vector2 current_position = current_transform.get_origin();
		Vector2 linear = (position - current_position) / last_step;

		b2Rot target_rotation = b2MakeRot(rotation);
		b2Rot current_rotation = b2MakeRot(current_transform.get_rotation());
		float angular = b2RelativeAngle(target_rotation, current_rotation) / last_step;

		b2Body_SetLinearVelocity(body_id, to_box2d(linear));
		b2Body_SetAngularVelocity(body_id, (float)angular);
	} else {
		b2Body_SetTransform(body_id, to_box2d(position), b2MakeRot(rotation));
		b2Body_SetAwake(body_id, true);
	}

	bool scale_changed = !current_transform.get_scale().is_equal_approx(p_transform.get_scale());
	bool skew_changed = !Math::is_equal_approx(current_transform.get_skew(), p_transform.get_skew());

	current_transform = p_transform;

	if (scale_changed || skew_changed) {
		rebuild_all_shapes();
	}
}

RID Box2DCollisionObject2D::get_shape_rid(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), RID());
	const Box2DShapeInstance &shape = shapes[p_index];
	Box2DShape2D *inst = shape.get_shape();
	ERR_FAIL_COND_V(!inst, RID());
	return inst->get_rid();
}

void Box2DCollisionObject2D::set_shape_disabled(int p_index, bool p_disabled) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Box2DShapeInstance &shape = shapes[p_index];
	if (shape.disabled == p_disabled) {
		return;
	}
	shape.disabled = p_disabled;
	build_shape(shape, true);
}

void Box2DCollisionObject2D::build_shape(Box2DShapeInstance &p_shape, bool p_shapes_changed) {
	if (!in_space) {
		return;
	}

	ERR_FAIL_COND(space->locked);

	p_shape.build();

	if (p_shapes_changed) {
		shapes_changed();
	}
}

void Box2DCollisionObject2D::rebuild_all_shapes() {
	for (Box2DShapeInstance &shape : shapes) {
		build_shape(shape, false);
	}

	shapes_changed();
}

void Box2DCollisionObject2D::add_shape(Box2DShape2D *p_shape, const Transform2D &p_transform, bool p_disabled) {
	shapes.push_back(Box2DShapeInstance(this, p_shape, p_transform, p_disabled));

	build_shape(shapes[shapes.size() - 1], true);

	reindex_all_shapes();
}

void Box2DCollisionObject2D::set_shape(int p_index, Box2DShape2D *p_shape) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Box2DShapeInstance &shape = shapes[p_index];
	shape.set_shape(p_shape);
	build_shape(shape, true);
}

void Box2DCollisionObject2D::shape_updated(Box2DShape2D *p_shape) {
	ERR_FAIL_NULL(p_shape);

	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i].get_shape() == p_shape) {
			build_shape(shapes[i]);
		}
	}

	shapes_changed();
}

void Box2DCollisionObject2D::remove_shape(int p_index) {
	ERR_FAIL_INDEX(p_index, shapes.size());

	on_shape_destroy(shapes[p_index]);
	shapes.remove_at(p_index);

	reindex_all_shapes();

	shapes_changed();
}

void Box2DCollisionObject2D::remove_shape(Box2DShape2D *p_shape) {
	ERR_FAIL_NULL(p_shape);

	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i].get_shape() == p_shape) {
			on_shape_destroy(shapes[i]);
			shapes.remove_at(i);
			i--;
		}
	}

	reindex_all_shapes();

	shapes_changed();
}

void Box2DCollisionObject2D::clear_shapes() {
	shapes.clear();
	shapes_changed();
}

void Box2DCollisionObject2D::reindex_all_shapes() {
	int index = 0;
	for (Box2DShapeInstance &shape : shapes) {
		shape.index = index;
		index++;
	}
}

void Box2DCollisionObject2D::set_shape_transform(int p_index, const Transform2D &p_transform) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	Box2DShapeInstance &shape = shapes[p_index];
	if (shape.transform == p_transform) {
		return;
	}
	shape.transform = p_transform;
	build_shape(shape, true);
}

Transform2D Box2DCollisionObject2D::get_shape_transform(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, shapes.size(), Transform2D());
	// TODO: more const references and pointers
	const Box2DShapeInstance &shape = shapes[p_index];
	return shape.transform;
}