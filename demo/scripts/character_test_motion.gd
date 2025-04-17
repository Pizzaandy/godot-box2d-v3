extends RigidBody2D

func _physics_process(delta: float) -> void:
	var input_vec = Input.get_vector("move_left", "move_right", "move_up", "move_down")

	global_position += 300 * input_vec * delta

	if Input.is_action_just_pressed("ui_accept"):
		var params = PhysicsTestMotionParameters2D.new()
		params.margin = 0
		params.from = global_transform
		params.recovery_as_collision = true
		#params.motion = input_vec * 500 * delta
		var result := PhysicsTestMotionResult2D.new()
		PhysicsServer2D.body_test_motion(get_rid(), params, result)
		global_position += result.get_travel()
