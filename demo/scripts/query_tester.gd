# @tool
extends Node2D

var circle_rid: RID
var capsule_rid: RID
var rectangle_rid: RID
var polygon_rid: RID

var shape_color: Color
var query: Query = Query.RAY

var collide_areas = true
var collide_bodies = true

var polygon_points = [
	Vector2(-30, 30), Vector2(30, 30),
	Vector2(30, 30), Vector2(30, -30),
	Vector2(30, -30), Vector2(-30, -30),
	Vector2(-30, -30), Vector2(-30, 30)
]

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

	polygon_rid = PhysicsServer2D.concave_polygon_shape_create()
	PhysicsServer2D.shape_set_data(polygon_rid, PackedVector2Array(polygon_points))

	shape_color = Color.DIM_GRAY
	shape_color.a = 0.6


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())

	var ray_count = 32 if query != Query.RAY else 1000
	for i in range(ray_count):
		var angle = i * (2 * PI / ray_count)
		var ray = Vector2.from_angle(angle) * 2000
		match query:
			Query.RAY:
				cast_ray(ray)
			Query.CIRCLE:
				var point = cast_shape(ray, circle_rid)
				draw_circle(point, 30, shape_color)
			Query.RECTANGLE:
				var point = cast_shape(ray, rectangle_rid)
				draw_rect(Rect2(point - Vector2(30, 30), Vector2(60, 60)), shape_color)
			Query.POLYGON:
				var point = cast_shape(ray, polygon_rid)
				for j in range(0, polygon_points.size(), 2):
					var point_a = point + polygon_points[j]
					var point_b = point + polygon_points[j + 1]
					draw_line(point_a, point_b, shape_color, 2.0)


func cast_ray(ray: Vector2):
	var parameters = PhysicsRayQueryParameters2D.new()
	parameters.from = global_position
	parameters.to = global_position + ray
	parameters.hit_from_inside = true
	var result = get_world_2d().direct_space_state.intersect_ray(parameters)
	if result:
		#draw_line(result["position"], result["position"] + result["normal"] * 25, Color.RED)
		draw_line(global_position, result["position"], Color.BLACK)
	else:
		draw_line(global_position, global_position + ray, Color.BLACK)


func cast_shape(ray: Vector2, shape_rid: RID) -> Vector2:
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = shape_rid
	parameters.collide_with_bodies = collide_bodies
	parameters.collide_with_areas = collide_areas
	var fractions = get_world_2d().direct_space_state.cast_motion(parameters)
	if not fractions:
		return parameters.transform.origin
	var end_point = global_position + (ray * fractions[0])
	draw_line(global_position, end_point, Color.BLACK)
	return end_point


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


func _on_polygon_pressed() -> void:
	query = Query.POLYGON
