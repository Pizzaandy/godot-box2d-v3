#pragma once

#include "../box2d_globals.h"
#include "box2d_physics_direct_space_state_2d.h"
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/rid.hpp>

using namespace godot;

class Box2DPhysicsDirectSpaceState2D;

class Box2DSpace2D {
public:
	Box2DSpace2D();
	~Box2DSpace2D();

	void step(float p_step);

	void sync_state();

	RID get_rid() const { return rid; }

	void set_rid(const RID &p_rid) { rid = p_rid; }

	Box2DPhysicsDirectSpaceState2D *get_direct_state();

	b2WorldId get_world_id() const { return world_id; }

	float get_last_step() const { return last_step; }

	int get_debug_contact_count() { return debug_contact_count; };
	PackedVector2Array &get_debug_contacts() { return debug_contacts; };
	void set_max_debug_contacts(int p_count) { debug_contacts.resize(p_count); }
	void add_debug_contact(Vector2 p_contact) {
		if (debug_contact_count < debug_contacts.size()) {
			debug_contacts[debug_contact_count++] = p_contact;
		}
	}

	int max_tasks = -1;
	bool locked = false;
	LocalVector<void *> delete_after_sync;

private:
	b2WorldId world_id = b2_nullWorldId;
	RID rid;
	Box2DPhysicsDirectSpaceState2D *direct_state = nullptr;
	float last_step = -1.0;
	b2ContactEvents contact_events;
	PackedVector2Array debug_contacts;
	int debug_contact_count = 0;
	int substeps = 4;
};