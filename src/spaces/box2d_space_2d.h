#pragma once

#include "../bodies/box2d_area_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../box2d_globals.h"
#include "box2d_physics_direct_space_state_2d.h"
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/rid.hpp>

using namespace godot;

struct Box2DTaskData {
	WorkerThreadPool::GroupID group_id;
	void *task_context;
	b2TaskCallback *task;
	int32_t item_count;
	int32_t task_count;
};

class Box2DDirectSpaceState2D;

class Box2DSpace2D {
public:
	Box2DSpace2D();
	~Box2DSpace2D();

	void step(float p_step);

	int get_max_tasks() const { return max_tasks; }

	void sync_state();

	RID get_rid() const { return rid; }

	void set_rid(const RID &p_rid) { rid = p_rid; }

	Box2DDirectSpaceState2D *get_direct_state();

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

	void set_default_area(Box2DArea2D *p_area) { default_area = p_area; }
	Box2DArea2D *get_default_area() const { return default_area; }

	void set_default_gravity(Vector2 p_gravity);
	Vector2 get_default_gravity() { return default_gravity; }

	void default_area_linear_damp_changed() { linear_damp_changed = true; }
	void default_area_angular_damp_changed() { angular_damp_changed = true; }

	void add_active_area(Box2DArea2D *p_area) {
		areas_to_step.ordered_insert(p_area);
	}
	void remove_active_area(Box2DArea2D *p_area) {
		areas_to_step.erase(p_area);
	}

	void add_constant_force_body(Box2DBody2D *p_body) {
		constant_force_list.push_back(p_body);
	}
	void remove_constant_force_body(Box2DBody2D *p_body) {
		constant_force_list.erase(p_body);
	}

	void add_force_integration_body(Box2DBody2D *p_body) {
		force_integration_list.push_back(p_body);
	}
	void remove_force_integration_body(Box2DBody2D *p_body) {
		force_integration_list.erase(p_body);
	}

	void add_body_with_overrides(Box2DBody2D *p_body) {
		bodies_with_overrides.insert(p_body);
	}

	bool locked = false;

private:
	LocalVector<Box2DBody2D *> constant_force_list;
	LocalVector<Box2DBody2D *> force_integration_list;
	LocalVector<Box2DArea2D *> areas_to_step;
	HashSet<Box2DBody2D *> bodies_with_overrides;

	Box2DArea2D *default_area = nullptr;
	Box2DDirectSpaceState2D *direct_state = nullptr;

	b2WorldId world_id = b2_nullWorldId;
	RID rid;
	float last_step = -1.0;
	b2ContactEvents contact_events;
	PackedVector2Array debug_contacts;
	int debug_contact_count = 0;
	int max_tasks = -1;
	int substeps = 4;
	Vector2 default_gravity = Vector2();

	bool linear_damp_changed = false;
	bool angular_damp_changed = false;
};