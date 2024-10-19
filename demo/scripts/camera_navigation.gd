extends Camera2D

@export_range(1, 20, 0.01) var maxZoom : float = 10.0
@export_range(0.01, 1, 0.01) var minZoom : float = 0.1
@export_range(0.01, 0.2, 0.01) var zoomStepRatio : float = 0.1

@onready var zoom_scale: float = clamp(zoom.x, minZoom, maxZoom)

func _input(event: InputEvent) -> void:
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_MIDDLE) or Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		var motion = event as InputEventMouseMotion
		if motion != null:
			global_position -= motion.relative / zoom_scale

	var prev_zoom = zoom_scale
	var scroll = event as InputEventMouseButton
	if scroll != null:
		if scroll.button_index == MOUSE_BUTTON_WHEEL_UP:
			zoom_scale += zoom_scale * zoomStepRatio
		elif scroll.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			zoom_scale -= zoom_scale * zoomStepRatio
		else:
			return
	else:
		return

	zoom_scale = clamp(zoom_scale, minZoom, maxZoom)

	var zoom_ratio = prev_zoom / zoom_scale
	global_position -= (get_global_mouse_position() - global_position) * (zoom_ratio - 1)

	zoom = Vector2.ONE * zoom_scale
