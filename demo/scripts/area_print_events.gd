extends Area2D

func _ready() -> void:
	body_entered.connect(on_body_entered)
	body_exited.connect(on_body_exited)
	body_shape_entered.connect(on_body_shape_entered)
	body_shape_exited.connect(on_body_shape_exited)
	area_entered.connect(on_area_entered)
	area_exited.connect(on_area_exited)


func on_body_shape_entered(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape entered, node: %s, shape: %s" % [body.name, body_shape_index])


func on_body_shape_exited(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape exited, node: %s, shape: %s" % [body.name if body else "null", body_shape_index])


func on_body_entered(body: Node2D) -> void:
	change_shape_colors(body, Color.RED)


func on_body_exited(body: Node2D) -> void:
	change_shape_colors(body, Color.BLUE)


func on_area_entered(area: Area2D) -> void:
	change_shape_colors(area, Color.RED)


func on_area_exited(area: Area2D) -> void:
	change_shape_colors(area, Color.BLUE)


func change_shape_colors(body: Node2D, color: Color) -> void:
	for shape in body.find_children("*", "CollisionShape2D", false):
		shape = shape as CollisionShape2D
		color.a = shape.debug_color.a
		shape.debug_color = color
