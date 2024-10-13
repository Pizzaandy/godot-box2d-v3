extends Node2D

@export var max_capsules = 100

var count = 0
var capsule = preload("res://capsule.tscn")
var has_stepped = false

func _physics_process(delta: float) -> void:
	if count >= max_capsules:
		return
	for i in 5:
		var inst = capsule.instantiate()
		get_parent().add_child(inst)
		inst.global_position = global_position + Vector2.from_angle(randf() * 2 * PI) * randf_range(0, 30)
		count += 1
