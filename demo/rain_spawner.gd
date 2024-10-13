@tool
extends Node2D

@export var max_bodies = 1000
@export var body: PackedScene
@export var range: Vector2 = Vector2(-400, 400)

var count = 0

func _physics_process(delta: float) -> void:
	if Engine.is_editor_hint():
		return

	if count >= max_bodies:
		return

	for i in 5:
		var inst = body.instantiate()
		get_parent().add_child(inst)
		inst.global_position = global_position + Vector2.RIGHT * randf_range(range.x, range.y)
		count += 1


func _process(delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	draw_dashed_line(global_position + Vector2.RIGHT * range.x, global_position + Vector2.RIGHT * range.y, Color.BLACK)
