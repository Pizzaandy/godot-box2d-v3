extends ItemList

var current_scene = null
@onready var settings_container: SettingsPanelFinder = $"../../../SettingsRoot/Panel/SettingsContainer"

func _on_item_selected(index: int) -> void:
	var scene_name := get_item_text(index)
	goto_scene(scene_name)


func goto_scene(name: String):
	var path: String = "res://%s.tscn" % name.strip_edges()
	if not ResourceLoader.exists(path):
		push_error("Scene %s does not exist" % name)
	_deferred_goto_scene.call_deferred(path)


func _deferred_goto_scene(path):
	if current_scene:
		current_scene.free()
	var s = ResourceLoader.load(path)
	current_scene = s.instantiate()
	get_tree().root.add_child(current_scene)
	get_tree().current_scene = current_scene
	settings_container.find_settings()
