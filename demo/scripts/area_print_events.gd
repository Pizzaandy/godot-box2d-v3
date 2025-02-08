extends Area2D


func _on_body_shape_entered(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape entered")


func _on_body_shape_exited(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape exited")


func _on_body_entered(body: Node2D) -> void:
	change_shape_colors(body, Color.RED)


func _on_body_exited(body: Node2D) -> void:
	change_shape_colors(body, Color.BLUE)


func change_shape_colors(body: Node2D, color: Color) -> void:
	for shape in body.find_children("*", "CollisionShape2D", false):
		shape = shape as CollisionShape2D
		color.a = shape.debug_color.a
		shape.debug_color = color
