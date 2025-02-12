extends Node2D


func _input(event):
	var ev = event as InputEventMouseButton
	if ev and ev.pressed and ev.button_index == MOUSE_BUTTON_LEFT:
		var space_state = get_world_2d().direct_space_state
		var query = PhysicsPointQueryParameters2D.new()
		query.position = get_global_mouse_position()
		query.collide_with_bodies = true
		query.collide_with_areas = true

		var results = space_state.intersect_point(query)
		if results.size() == 0:
			return

		var rigidbodies = results.filter(func(result): return result.collider is RigidBody2D)
		if rigidbodies.size() == 0:
			results[0].collider.queue_free()
		else:
			rigidbodies[0].collider.queue_free()
