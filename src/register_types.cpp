#include "register_types.h"
#include "bodies/box2d_direct_body_state_2d.h"
#include "box2d_project_settings.h"
#include "servers/box2d_physics_server_2d.h"
#include <gdextension_interface.h>
#include <godot_cpp/classes/physics_server2d_manager.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

Box2DPhysicsServer2D *create_box2d_physics_server() {
	return memnew(Box2DPhysicsServer2D);
}

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_SERVERS: {
			ClassDB::register_class<Box2DDirectBodyState2D>(true);
			ClassDB::register_class<Box2DDirectSpaceState2D>(true);
			ClassDB::register_class<Box2DPhysicsServer2D>();

			PhysicsServer2DManager::get_singleton()->register_server(
					"Box2D",
					callable_mp_static(&create_box2d_physics_server));
		} break;
		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			Box2DProjectSettings::register_settings();
		} break;
		default:
			break;
	}
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT godot_box2d_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
