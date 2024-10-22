extends MarginContainer

class_name SettingsPanelFinder

func find_settings() -> void:
	for child in get_children():
		child.free()
	var settings = get_tree().root.find_child("_Settings", true, false)
	if settings:
		settings.reparent(self, false)
