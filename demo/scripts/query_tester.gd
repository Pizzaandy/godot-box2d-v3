# @tool
extends Node2D

var circle_rid: RID


func _ready() -> void:
	circle_rid = PhysicsServer2D.circle_shape_create()
	PhysicsServer2D.shape_set_data(circle_rid, 30)


func _physics_process(delta: float) -> void:
	global_position = get_global_mouse_position()
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	var ray_count = 1
	for i in range(ray_count):
		var angle = i * (2 * PI / ray_count)
		# test_raycast(Vector2.from_angle(angle) * 2000)
		test_shapecast(circle_rid, Vector2.from_angle(angle) * 2000)


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


func test_shapecast(shape: RID, ray: Vector2):
	var parameters = PhysicsShapeQueryParameters2D.new()
	parameters.transform = global_transform
	parameters.motion = ray
	parameters.shape_rid = shape
	var fractions = get_world_2d().direct_space_state.cast_motion(parameters)
	draw_line(global_position, global_position + (ray * fractions[0]), Color.BLACK)


func test_point():
	var parameters = PhysicsPointQueryParameters2D.new()
	parameters.position = global_position
	var results = get_world_2d().direct_space_state.intersect_point(parameters)
