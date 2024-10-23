# @tool
extends Node2D

var circle_rid: RID
var capsule_rid: RID
var rectangle_rid: RID

var shape_color: Color
var query: Query = Query.RAY

enum Query {
	RAY,
	CIRCLE,
	CAPSULE,
	RECTANGLE,
	POLYGON,
}

func _ready() -> void:
	circle_rid = PhysicsServer2D.circle_shape_create()
	PhysicsServer2D.shape_set_data(circle_rid, 30)

	capsule_rid = PhysicsServer2D.capsule_shape_create()
	PhysicsServer2D.shape_set_data(capsule_rid, [80, 30])

	rectangle_rid = PhysicsServer2D.rectangle_shape_create()
	PhysicsServer2D.shape_set_data(rectangle_rid, Vector2(30, 30))

	shape_color = Color.DIM_GRAY
	shape_color.a = 0.6


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())

	var ray_count = 16 if query != Query.RAY else 500
	for i in range(ray_count):
		var angle = i * (2 * PI / ray_count)
		var ray = Vector2.from_angle(angle) * 2000
		match query:
			Query.RAY:
				test_raycast(ray)
			Query.CIRCLE:
				test_circle(ray)
			Query.RECTANGLE:
				test_rectangle(ray)


func test_raycast(ray: Vector2):
	var parameters = PhysicsRayQueryParameters2D.new()
	parameters.from = global_position
	parameters.to = global_position + ray
	parameters.hit_from_inside = true
	var result = get_world_2d().direct_space_state.intersect_ray(parameters)
	if result:
		draw_line(result["position"], result["position"] + result["normal"] * 25, Color.RED)
		draw_line(global_position, result["position"], Color.BLACK)
	else:
		draw_line(global_position, global_position + ray, Color.BLACK)


func test_circle(ray: Vector2):
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = circle_rid
	var fractions = get_world_2d().direct_space_state.cast_motion(parameters)
	if not fractions:
		return
	draw_line(global_position, global_position + (ray * fractions[0]), Color.BLACK)
	draw_circle(global_position + (ray * fractions[0]), 30, shape_color)


func test_rectangle(ray: Vector2):
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = rectangle_rid
	var fractions = get_world_2d().direct_space_state.cast_motion(parameters)
	if not fractions:
		return
	var end_point = global_position + (ray * fractions[0])
	draw_line(global_position, end_point, Color.BLACK)
	draw_rect(Rect2(end_point - Vector2(30, 30), Vector2(60, 60)), shape_color)


func test_rest_info():
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.shape_rid = circle_rid
	var result = get_world_2d().direct_space_state.get_rest_info(parameters)
	draw_circle(global_position, 30, shape_color)
	if not result:
		return
	draw_line(result["point"], result["point"] + result["normal"] * 25, Color.RED)
	draw_circle(result["point"], 3, Color.ORANGE)


func test_point():
	var parameters = PhysicsPointQueryParameters2D.new()
	parameters.position = global_position
	var results = get_world_2d().direct_space_state.intersect_point(parameters)


func _on_raycast_pressed() -> void:
	query = Query.RAY


func _on_circle_pressed() -> void:
	query = Query.CIRCLE


func _on_rectangle_pressed() -> void:
	query = Query.RECTANGLE
