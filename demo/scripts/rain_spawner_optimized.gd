@tool
extends Node2D

@export var enabled = true
@export var max_bodies = 10_000
@export var spawn_range: Vector2 = Vector2(-400, 400)

var count = 0
var multimesh: MultiMesh
var ball_texture = preload("res://sprites/circle.png")
var shape_rid: RID
var bodies: Array[RID]

func _ready() -> void:
	shape_rid = Box2DPhysicsServer2D.circle_shape_create()
	Box2DPhysicsServer2D.shape_set_data(shape_rid, 25)
	multimesh = MultiMesh.new()
	multimesh.transform_format = MultiMesh.TRANSFORM_2D
	multimesh.instance_count = max_bodies
	multimesh.visible_instance_count = 0
	var quadmesh = QuadMesh.new()
	quadmesh.size = Vector2(50, 50)
	multimesh.mesh = quadmesh


func _physics_process(delta: float) -> void:
	if Engine.is_editor_hint():
		return

	var event_data := Box2DPhysicsServer2D.space_get_body_events(get_world_2d().space)

	for i in range(0, event_data.size(), 2):
		var index = event_data[i]
		if not index:
			continue
		var xform = event_data[i + 1]
		multimesh.set_instance_transform_2d(index, xform)

	if count >= max_bodies or not enabled:
		return

	for i in 20:
		var pos = global_position + Vector2.RIGHT * randf_range(spawn_range.x, spawn_range.y)
		spawn_ball(pos)
		count += 1
		multimesh.visible_instance_count += 1


func spawn_ball(pos: Vector2):
	var rid = Box2DPhysicsServer2D.body_create()

	Box2DPhysicsServer2D.body_set_user_data(rid, count)
	Box2DPhysicsServer2D.body_set_mode(rid, PhysicsServer2D.BodyMode.BODY_MODE_RIGID)
	Box2DPhysicsServer2D.body_add_shape(rid, shape_rid)
	Box2DPhysicsServer2D.body_set_state(rid, PhysicsServer2D.BodyState.BODY_STATE_TRANSFORM, Transform2D(0, pos))
	Box2DPhysicsServer2D.body_set_space(rid, get_world_2d().space)

	bodies.push_back(rid)


func _process(delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	draw_line(global_position + Vector2.RIGHT * spawn_range.x, global_position + Vector2.RIGHT * spawn_range.y, Color.BLACK)
	if multimesh:
		draw_multimesh(multimesh, ball_texture)


func _exit_tree() -> void:
	for rid in bodies:
		Box2DPhysicsServer2D.free_rid(rid)
