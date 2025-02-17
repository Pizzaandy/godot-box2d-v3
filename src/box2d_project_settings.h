#pragma once

#include <godot_cpp/classes/project_settings.hpp>

using namespace godot;

enum MixingRule : int32_t {
	MIXING_RULE_GODOT,
	MIXING_RULE_BOX2D,
	MIXING_RULE_MAX,
};

class Box2DProjectSettings {
public:
	static void register_settings();

	static int32_t get_max_threads();

	static int get_pixels_per_meter();

	static int get_substeps();

	static float get_contact_hertz();

	static float get_contact_damping_ratio();

	static float get_joint_hertz();

	static float get_joint_damping_ratio();

	static MixingRule get_friction_mixing_rule();

	static MixingRule get_restitution_mixing_rule();

	static bool get_presolve_enabled();
};