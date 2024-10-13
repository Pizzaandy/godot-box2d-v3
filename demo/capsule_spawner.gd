extends Node2D

@export var max_capsules = 100

var count = 0
var body = preload("res://robot.tscn")

func _physics_process(delta: float) -> void:
	if count >= max_capsules:
		return
	for i in 1:
		var inst = body.instantiate()
		get_parent().add_child(inst)
		inst.global_position = global_position + Vector2.from_angle(randf() * 2 * PI) * randf_range(0, 30)
		count += 1
