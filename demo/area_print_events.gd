extends Area2D

func _on_body_shape_entered(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape entered")


func _on_body_shape_exited(body_rid: RID, body: Node2D, body_shape_index: int, local_shape_index: int) -> void:
	print("shape exited")


func _on_body_entered(body: Node2D) -> void:
	print("body entered")


func _on_body_exited(body: Node2D) -> void:
	print("body exited")
