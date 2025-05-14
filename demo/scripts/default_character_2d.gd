extends CharacterBody2D


const SPEED = 300.0
const JUMP_VELOCITY = -400.0


func _physics_process(delta: float) -> void:
	# Add the gravity.
	if not is_on_floor():
		velocity += get_gravity() * delta

	# Handle jump.
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = JUMP_VELOCITY

	# Get the input direction and handle the movement/deceleration.
	# As good practice, you should replace UI actions with custom gameplay actions.
	var direction := Input.get_axis("ui_left", "ui_right")
	if direction:
		velocity.x = direction * SPEED
	else:
		velocity.x = move_toward(velocity.x, 0, SPEED)

	move_and_slide()
	queue_redraw()

func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())

	var last_collision = get_last_slide_collision()
	if not last_collision:
		return
	var point := last_collision.get_position()
	var normal := last_collision.get_normal()
	draw_circle(point, 3.0, Color.BLACK)
	draw_line(point, point + normal * 25, Color.BLACK, 3)
