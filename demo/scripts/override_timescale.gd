extends Node2D

@export var time_scale: float = 1.0


func _enter_tree() -> void:
	Engine.time_scale = time_scale


func _exit_tree() -> void:
	Engine.time_scale = 1.0
