#pragma once

#include <godot_cpp/classes/project_settings.hpp>

using namespace godot;

class Box2DProjectSettings {
public:
	static void register_settings();

	static Vector2 get_default_gravity();

	static int32_t get_max_threads();
};