extends RigidBody2D

var contacts: Array[Vector2] = []

func _ready() -> void:
	max_contacts_reported = 8


func _integrate_forces(state: PhysicsDirectBodyState2D) -> void:
	contacts.clear()

	for i in state.get_contact_count():
		contacts.push_back(state.get_contact_local_position(i))

	print(contacts.size())
	queue_redraw()


func _draw() -> void:
	draw_set_transform_matrix(global_transform.affine_inverse())
	for contact in contacts:
		draw_circle(contact, 5.0, Color.RED)
