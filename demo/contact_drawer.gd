extends RigidBody2D

var contacts: Array[Vector2] = []

func _ready() -> void:
	max_contacts_reported = 8


func _integrate_forces(state: PhysicsDirectBodyState2D) -> void:
	contacts.clear()

	for i in state.get_contact_count():
		contacts.push_back(state.get_contact_local_position(i))


func _process(delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	var zoom_scale = get_viewport_transform().affine_inverse().get_scale().x

	for contact in contacts:
		draw_circle(contact, zoom_scale * 4.0, Color.RED)
