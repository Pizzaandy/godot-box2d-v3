@tool
extends Node2D

@export var rows: int = 10
@export var columns: int = 10
@export var cell_length: float = 20
@export var body: PackedScene

func _ready() -> void:
	if Engine.is_editor_hint():
		return

	if body == null:
		return

	for i in rows:
		for j in columns:
			var inst = body.instantiate()
			get_parent().add_child.call_deferred(inst)
			inst.global_position = global_position + Vector2(i * cell_length, j * cell_length)


func _process(delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	if not Engine.is_editor_hint():
		return
	for i in rows:
		for j in columns:
			var pos = global_position + Vector2(i * cell_length, j * cell_length)
			draw_circle(to_local(pos), 5, Color.BLACK)
