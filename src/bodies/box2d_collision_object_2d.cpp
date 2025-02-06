#include "box2d_collision_object_2d.h"

#include "../spaces/box2d_space_2d.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void Box2DCollisionObject2D::destroy_body() {
	if (b2Body_IsValid(body_id)) {
		on_destroy_body();
		b2DestroyBody(body_id);
	}

	body_exists = false;
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

	body_exists = true;

	rebuild_all_shapes();

	on_body_created();
}

void Box2DCollisionObject2D::set_mode(PhysicsServer2D::BodyMode p_mode) {
	PhysicsServer2D::BodyMode previous_mode = mode;
	mode = p_mode;

	switch (p_mode) {
		case PhysicsServer2D::BODY_MODE_STATIC:
			body_def.type = b2_staticBody;
			if (!body_exists) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_staticBody);
			break;
		case PhysicsServer2D::BODY_MODE_KINEMATIC:
			body_def.type = b2_kinematicBody;
			if (!body_exists) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_kinematicBody);
			break;
		case PhysicsServer2D::BODY_MODE_RIGID:
			body_def.type = b2_dynamicBody;
			body_def.fixedRotation = false;
			if (!body_exists) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, false);
			shapes_changed();
			break;
		case PhysicsServer2D::BODY_MODE_RIGID_LINEAR:
			body_def.type = b2_dynamicBody;
			body_def.fixedRotation = true;
			if (!body_exists) {
				return;
			}
			b2Body_SetType(body_id, b2BodyType::b2_dynamicBody);
			b2Body_SetFixedRotation(body_id, true);
			shapes_changed();
			break;
		default:
			return;
	}
}

void Box2DCollisionObject2D::set_collision_layer(uint32_t p_layer) {
	shape_def.filter.categoryBits = p_layer;

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DCollisionObject2D::set_collision_mask(uint32_t p_mask) {
	// TODO: Use a virtual method
	shape_def.filter.maskBits = p_mask | (is_area() ? BODY_MASK_BIT : AREA_MASK_BIT);

	if (!body_exists) {
		return;
	}

	BodyShapeRange range(body_id);
	for (b2ShapeId id : range) {
		b2Shape_SetFilter(id, shape_def.filter);
	}
}

void Box2DCollisionObject2D::set_transform(const Transform2D &p_transform, bool p_move_kinematic) {
	if (!body_exists) {
		current_transform = p_transform;
		return;
	}

	if (p_transform == current_transform) {
		return;
	}

	Vector2 position = p_transform.get_origin();
	float rotation = p_transform.get_rotation();
	float last_step = space->get_last_step();

	if (is_area()) {
		p_move_kinematic = false;
	}

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
	Box2DShape2D *inst = shape.get_shape_or_null();
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
	if (!body_exists) {
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

void Box2DCollisionObject2D::remove_shape(int p_index) {
	ERR_FAIL_INDEX(p_index, shapes.size());

	shapes.remove_at(p_index);

	reindex_all_shapes();

	shapes_changed();
}

void Box2DCollisionObject2D::remove_shape(Box2DShape2D *p_shape) {
	ERR_FAIL_NULL(p_shape);

	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i].get_shape_or_null() == p_shape) {
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
	const Box2DShapeInstance &shape = shapes[p_index];
	return shape.transform;
}