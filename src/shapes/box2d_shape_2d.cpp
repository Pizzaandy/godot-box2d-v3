#include "box2d_shape_2d.h"
#include "../bodies/box2d_collision_object_2d.h"

Box2DShape2D::~Box2DShape2D() {
	remove_from_all_objects();
}

void Box2DShape2D::remove_from_body(Box2DShapeInstance *p_instance) const {
	for (b2ShapeId shape_id : p_instance->get_shape_ids()) {
		if (b2Shape_IsValid(shape_id)) {
			b2DestroyShape(shape_id, false);
		}
	}
	p_instance->clear_shape_ids();
}

void Box2DShape2D::set_data(const Variant &p_data) {
	data = p_data;
	update_instances();
}

void Box2DShape2D::update_instances() {
	for (const auto &[owner, ref_count] : ref_counts_by_object) {
		owner->shape_updated(this);
	}
}

void Box2DShape2D::add_object_reference(Box2DCollisionObject2D *p_owner) {
	ref_counts_by_object[p_owner]++;
}

void Box2DShape2D::remove_object_reference(Box2DCollisionObject2D *p_owner) {
	if (--ref_counts_by_object[p_owner] <= 0) {
		ref_counts_by_object.erase(p_owner);
	}
}

void Box2DShape2D::remove_from_all_objects() {
	const auto ref_counts_by_owner_copy = ref_counts_by_object;
	for (const auto &[owner, ref_count] : ref_counts_by_owner_copy) {
		owner->remove_shape(this);
	}
}