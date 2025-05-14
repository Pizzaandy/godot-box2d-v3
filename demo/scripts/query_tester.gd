# @tool
extends Node2D

var selected_shape_rid: RID
var circle_rid: RID
var capsule_rid: RID
var rectangle_rid: RID
var polygon_rid: RID

var query: QueryType = QueryType.RAYCAST

var shape_color: Color
var shape_type: PhysicsServer2D.ShapeType = PhysicsServer2D.ShapeType.SHAPE_CIRCLE

var collide_areas = true
var collide_bodies = true

var polygon_points = [
	Vector2(0, -40),
	Vector2(30, -30),
	Vector2(40, 0),
	Vector2(30, 30),
	Vector2(0, 40),
	Vector2(-25, 30),
	Vector2(-40, 0),
	Vector2(-25, -30)
]

enum QueryType {
	RAYCAST,
	CAST_MOTION,
	COLLIDE_SHAPE,
	REST_INFO,
}

func _ready() -> void:
	circle_rid = PhysicsServer2D.circle_shape_create()
	PhysicsServer2D.shape_set_data(circle_rid, 30)

	selected_shape_rid = circle_rid

	capsule_rid = PhysicsServer2D.capsule_shape_create()
	PhysicsServer2D.shape_set_data(capsule_rid, [80, 30])

	rectangle_rid = PhysicsServer2D.rectangle_shape_create()
	PhysicsServer2D.shape_set_data(rectangle_rid, Vector2(30, 30))

	polygon_rid = PhysicsServer2D.convex_polygon_shape_create()
	PhysicsServer2D.shape_set_data(polygon_rid, PackedVector2Array(polygon_points))

	shape_color = Color.DIM_GRAY
	shape_color.a = 0.6


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())

	match query:
		QueryType.RAYCAST:
			var ray_count = 1000
			for i in range(ray_count):
				var angle = i * (2 * PI / ray_count)
				var ray = Vector2.from_angle(angle) * 2000
				cast_ray(ray)
		QueryType.CAST_MOTION:
			var cast_count = 32
			for i in range(cast_count):
				var angle = i * (2 * PI / cast_count)
				var ray = Vector2.from_angle(angle) * 2000
				cast_motion(ray, selected_shape_rid)
		QueryType.COLLIDE_SHAPE:
			var cast_count = 1
			for i in range(cast_count):
				var angle = i * (2 * PI / cast_count)
				var ray = Vector2.from_angle(angle) * 300
				collide_shape(ray, selected_shape_rid)
		QueryType.REST_INFO:
			rest_info(selected_shape_rid)


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


func cast_motion(ray: Vector2, shape_rid: RID) -> void:
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = shape_rid
	parameters.collide_with_bodies = collide_bodies
	parameters.collide_with_areas = collide_areas
	var fractions = get_world_2d().direct_space_state.cast_motion(parameters)
	if not fractions:
		return
	var end_point = global_position + (ray * fractions[0])
	draw_shape(selected_shape_rid, end_point)
	draw_line(global_position, end_point, Color.BLACK)


func collide_shape(ray: Vector2, shape_rid: RID) -> void:
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = shape_rid
	parameters.collide_with_bodies = collide_bodies
	parameters.collide_with_areas = collide_areas
	var end_point = global_position + ray
	draw_shape(selected_shape_rid, end_point)
	draw_line(global_position, end_point, Color.BLACK)
	var points = get_world_2d().direct_space_state.collide_shape(parameters)
	var zoom_scale = get_viewport_transform().affine_inverse().get_scale().x
	for i in range(0, points.size(), 2):
		draw_circle(points[i], zoom_scale * 6.0, Color.ORANGE)
		draw_circle(points[i + 1], zoom_scale * 3.0, Color.ROYAL_BLUE)


func rest_info(shape_rid: RID) -> void:
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.shape_rid = shape_rid
	parameters.collide_with_bodies = collide_bodies
	parameters.collide_with_areas = collide_areas
	var result := get_world_2d().direct_space_state.get_rest_info(parameters)

	draw_shape(selected_shape_rid, global_position)
	if not result:
		return
	var zoom_scale = get_viewport_transform().affine_inverse().get_scale().x
	draw_circle(result["point"], zoom_scale * 6.0, Color.ORANGE)


func test_point():
	var parameters = PhysicsPointQueryParameters2D.new()
	parameters.position = global_position
	var results = get_world_2d().direct_space_state.intersect_point(parameters)


func _on_circle_pressed() -> void:
	# shape_type = ShapeType.CIRCLE
	selected_shape_rid = circle_rid

func _on_capsule_pressed() -> void:
	# shape_type = ShapeType.CAPSULE
	selected_shape_rid = capsule_rid

func _on_rectangle_pressed() -> void:
	# shape_type = ShapeType.RECTANGLE
	selected_shape_rid = rectangle_rid

func _on_polygon_pressed() -> void:
	# shape_type = ShapeType.POLYGON
	selected_shape_rid = polygon_rid


func draw_shape(rid: RID, pos: Vector2) -> void:
	var type := PhysicsServer2D.shape_get_type(rid)
	var data = PhysicsServer2D.shape_get_data(rid)
	match type:
		PhysicsServer2D.ShapeType.SHAPE_CIRCLE:
			draw_circle(pos, 30, shape_color)
		PhysicsServer2D.ShapeType.SHAPE_RECTANGLE:
			draw_rect(Rect2(pos - Vector2(30, 30), Vector2(60, 60)), shape_color)
		PhysicsServer2D.ShapeType.SHAPE_CONVEX_POLYGON:
			for j in polygon_points.size():
				var point_a = pos + polygon_points[j]
				var point_b = pos + polygon_points[(j + 1) % polygon_points.size()]
				draw_line(point_a, point_b, shape_color, 2.0)


func _on_intersect_ray_pressed() -> void:
	query = QueryType.RAYCAST


func _on_cast_motion_pressed() -> void:
	query = QueryType.CAST_MOTION


func _on_collide_shape_pressed() -> void:
	query = QueryType.COLLIDE_SHAPE


func _on_rest_info_pressed() -> void:
	query = QueryType.REST_INFO
