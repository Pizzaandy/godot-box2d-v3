extends AnimatableBody2D

func _physics_process(delta: float) -> void:
	global_rotation += PI * 1 * delta
