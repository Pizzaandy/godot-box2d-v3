extends Node2D

@export var max_capsules = 100
var count = 0
var capsule = preload("res://capsule.tscn")
var has_stepped = false

func _physics_process(delta: float) -> void:
	if count >= max_capsules:
		return
	var inst = capsule.instantiate()
	get_parent().add_child(inst)
	inst.global_position = global_position
	count += 1
