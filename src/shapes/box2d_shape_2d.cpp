#include "box2d_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

Box2DShape2D::~Box2DShape2D() {
	remove_from_all_bodies();
}

void Box2DShape2D::remove_from_body(b2BodyId p_body_id, Box2DShapeInstance *p_instance) const {
	for (b2ShapeId shape_id : p_instance->shape_ids) {
		b2DestroyShape(shape_id, false);
	}
}

void Box2DShape2D::set_data(const Variant &p_data) {
	data = p_data;
	update_instances();
}

void Box2DShape2D::update_instances() {
	for (const auto &[owner, ref_count] : ref_counts_by_body) {
		owner->shapes_changed();
	}
}

void Box2DShape2D::add_body_reference(Box2DCollisionObject2D *p_owner) {
	ref_counts_by_body[p_owner]++;
}

void Box2DShape2D::remove_body_reference(Box2DCollisionObject2D *p_owner) {
	if (--ref_counts_by_body[p_owner] <= 0) {
		ref_counts_by_body.erase(p_owner);
	}
}

void Box2DShape2D::remove_from_all_bodies() {
	const auto ref_counts_by_owner_copy = ref_counts_by_body;
	for (const auto &[owner, ref_count] : ref_counts_by_owner_copy) {
		owner->remove_shape(this);
	}
}