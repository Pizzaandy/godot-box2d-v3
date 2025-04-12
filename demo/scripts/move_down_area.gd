extends Area2D


func _physics_process(delta: float) -> void:
	global_position += Vector2.DOWN * 250 * delta
