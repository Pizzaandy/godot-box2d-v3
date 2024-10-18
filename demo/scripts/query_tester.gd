extends Node2D


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()

	if not Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		return

	var parameters = PhysicsPointQueryParameters2D.new()
	parameters.position = global_position
	var results = get_world_2d().direct_space_state.intersect_point(parameters)
	# print(results.size())

	for result in results:
		var shape: int = result["shape"]
		var collider := result["collider"] as CollisionObject2D
		var collision_shape := collider.shape_owner_get_owner(collider.shape_find_owner(shape)) as CollisionShape2D
		if collision_shape:
			collision_shape.debug_color = Color.from_hsv(1.0, 0.7, 0.7, collision_shape.debug_color.a)
