#include "box2d_project_settings.h"

constexpr char DEFAULT_GRAVITY[] = "physics/box_2d/default_gravity";
constexpr char SUBSTEPS[] = "physics/box_2d/substeps";
constexpr char PIXELS_PER_METER[] = "physics/box_2d/pixels_per_meter";
constexpr char MAX_THREADS[] = "threading/worker_pool/max_threads";

template <typename TType>
TType get_setting(const char *p_setting) {
	const ProjectSettings *project_settings = ProjectSettings::get_singleton();
	const Variant setting_value = project_settings->get_setting_with_override(p_setting);
	const Variant::Type setting_type = setting_value.get_type();
	const Variant::Type expected_type = Variant(TType()).get_type();

	ERR_FAIL_COND_V_MSG(
			setting_type != expected_type,
			{},
			vformat("Unexpected type for setting '%s'. Expected type '%s' but found '%s'.", p_setting, Variant::get_type_name(expected_type), Variant::get_type_name(setting_type)));

	return setting_value;
}

void register_setting(
		const String &p_name,
		const Variant &p_value,
		bool p_needs_restart,
		PropertyHint p_hint,
		const String &p_hint_string) {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	if (!project_settings->has_setting(p_name)) {
		project_settings->set(p_name, p_value);
	}

	Dictionary property_info;
	property_info["name"] = p_name;
	property_info["type"] = p_value.get_type();
	property_info["hint"] = p_hint;
	property_info["hint_string"] = p_hint_string;

	project_settings->add_property_info(property_info);
	project_settings->set_initial_value(p_name, p_value);
	project_settings->set_restart_if_changed(p_name, p_needs_restart);

	// HACK(mihe): We want our settings to appear in the order we register them in, but if we start
	// the order at 0 we end up moving the entire `physics/` group to the top of the tree view, so
	// instead we give it a hefty starting order and increment from there, which seems to give us
	// the desired effect.
	static int32_t order = 1000000;

	project_settings->set_order(p_name, order++);
}

void register_setting_plain(
		const String &p_name,
		const Variant &p_value,
		bool p_needs_restart = false) {
	register_setting(p_name, p_value, p_needs_restart, PROPERTY_HINT_NONE, {});
}

void register_setting_hinted(
		const String &p_name,
		const Variant &p_value,
		const String &p_hint_string,
		bool p_needs_restart = false) {
	register_setting(p_name, p_value, p_needs_restart, PROPERTY_HINT_NONE, p_hint_string);
}

void register_setting_ranged(
		const String &p_name,
		const Variant &p_value,
		const String &p_hint_string,
		bool p_needs_restart = false) {
	register_setting(p_name, p_value, p_needs_restart, PROPERTY_HINT_RANGE, p_hint_string);
}

void register_setting_enum(
		const String &p_name,
		const Variant &p_value,
		const String &p_hint_string,
		bool p_needs_restart = false) {
	register_setting(p_name, p_value, p_needs_restart, PROPERTY_HINT_ENUM, p_hint_string);
}

void Box2DProjectSettings::register_settings() {
	register_setting_plain(DEFAULT_GRAVITY, Vector2(0, 980));
	register_setting_ranged(SUBSTEPS, 4, U"1,8,or_greater");
	register_setting_ranged(PIXELS_PER_METER, 100, U"1,500,or_greater,suffix:px / m", true);
}

Vector2 Box2DProjectSettings::get_default_gravity() {
	static const auto value = get_setting<Vector2>(DEFAULT_GRAVITY);
	return value;
}

int Box2DProjectSettings::get_substeps() {
	static const auto value = get_setting<int>(SUBSTEPS);
	return value;
}

int Box2DProjectSettings::get_pixels_per_meter() {
	static const auto value = get_setting<int>(PIXELS_PER_METER);
	return value;
}

int32_t Box2DProjectSettings::get_max_threads() {
	static const auto value = get_setting<int32_t>(MAX_THREADS);
	return value;
}