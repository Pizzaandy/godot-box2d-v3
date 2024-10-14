extends Node2D


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()

	var parameters = PhysicsPointQueryParameters2D.new()
	parameters.position = global_position
	var results = get_world_2d().direct_space_state.intersect_point(parameters)
	print(results.size())
