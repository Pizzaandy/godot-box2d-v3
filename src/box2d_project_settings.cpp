#include "box2d_project_settings.h"

constexpr char SUBSTEPS[] = "physics/box_2d/substeps";
constexpr char PIXELS_PER_METER[] = "physics/box_2d/pixels_per_meter";

constexpr char CONTACT_HERTZ[] = "physics/box_2d/solver/contact_hertz";
constexpr char CONTACT_DAMPING_RATIO[] = "physics/box_2d/solver/contact_damping_ratio";
constexpr char JOINT_HERTZ[] = "physics/box_2d/solver/joint_hertz";
constexpr char JOINT_DAMPING_RATIO[] = "physics/box_2d/solver/joint_damping_ratio";
constexpr char FRICTION_MIXING_RULE[] = "physics/box_2d/solver/friction_mixing_rule";
constexpr char RESTITUTION_MIXING_RULE[] = "physics/box_2d/solver/restitution_mixing_rule";

constexpr char PRESOLVE_ENABLED[] = "physics/box_2d/advanced/presolve_enabled";

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
	register_setting_ranged(SUBSTEPS, 4, U"1,8,or_greater");
	register_setting_ranged(PIXELS_PER_METER, 100, U"1,500,or_greater,suffix:px / m", true);

	register_setting_plain(CONTACT_HERTZ, 30.0);
	register_setting_plain(CONTACT_DAMPING_RATIO, 10.0);
	register_setting_plain(JOINT_HERTZ, 60.0);
	register_setting_plain(JOINT_DAMPING_RATIO, 2.0);
	register_setting_enum(FRICTION_MIXING_RULE, MIXING_RULE_GODOT, "Minimum (Godot),Geometric Mean (Box2D)");
	register_setting_enum(RESTITUTION_MIXING_RULE, MIXING_RULE_GODOT, "Additive (Godot),Maximum (Box2D)");

	register_setting_plain(PRESOLVE_ENABLED, true, true);
}

int Box2DProjectSettings::get_pixels_per_meter() {
	static const auto value = get_setting<int>(PIXELS_PER_METER);
	return value;
}

int Box2DProjectSettings::get_substeps() {
	static const auto value = get_setting<int>(SUBSTEPS);
	return value;
}

float Box2DProjectSettings::get_contact_hertz() {
	static const auto value = get_setting<float>(CONTACT_HERTZ);
	return value;
}

float Box2DProjectSettings::get_contact_damping_ratio() {
	static const auto value = get_setting<float>(CONTACT_DAMPING_RATIO);
	return value;
}

float Box2DProjectSettings::get_joint_hertz() {
	static const auto value = get_setting<float>(JOINT_HERTZ);
	return value;
}

float Box2DProjectSettings::get_joint_damping_ratio() {
	static const auto value = get_setting<float>(JOINT_DAMPING_RATIO);
	return value;
}

bool Box2DProjectSettings::get_presolve_enabled() {
	static const auto value = get_setting<bool>(PRESOLVE_ENABLED);
	return value;
}

MixingRule Box2DProjectSettings::get_friction_mixing_rule() {
	static const auto value = get_setting<int32_t>(FRICTION_MIXING_RULE);
	ERR_FAIL_COND_V(value >= MIXING_RULE_MAX, MIXING_RULE_GODOT);
	return static_cast<MixingRule>(value);
}

MixingRule Box2DProjectSettings::get_restitution_mixing_rule() {
	static const auto value = get_setting<int32_t>(RESTITUTION_MIXING_RULE);
	ERR_FAIL_COND_V(value >= MIXING_RULE_MAX, MIXING_RULE_GODOT);
	return static_cast<MixingRule>(value);
}

int32_t Box2DProjectSettings::get_max_threads() {
	static const auto value = get_setting<int32_t>(MAX_THREADS);
	return value;
}