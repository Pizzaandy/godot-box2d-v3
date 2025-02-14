@tool
extends Node2D

@export var enabled = true
@export var max_bodies = 5000
@export var spawns_per_second = 1
@export var body: PackedScene

@export var spawn_range: Vector2 = Vector2(-400, 400)
@export var scale_range: Vector2 = Vector2(0.5, 1.5)

var count = 0
var timer = 0

func _physics_process(delta: float) -> void:
	if Engine.is_editor_hint():
		return

	if count >= max_bodies or not enabled:
		return

	timer += delta
	var interval = 1.0 / spawns_per_second
	while timer >= interval:
		timer -= interval
		var pos = global_position + Vector2.RIGHT * randf_range(spawn_range.x, spawn_range.y)
		spawn_object(pos)
		count += 1


func spawn_object(pos: Vector2):
	var inst: Node2D = body.instantiate()
	get_parent().add_child(inst)
	inst.global_position = pos
	inst.scale = randf_range(scale_range.x, scale_range.y) * Vector2.ONE
	inst.reset_physics_interpolation()


func _process(delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	draw_line(global_position + Vector2.RIGHT * spawn_range.x, global_position + Vector2.RIGHT * spawn_range.y, Color.BLACK)
