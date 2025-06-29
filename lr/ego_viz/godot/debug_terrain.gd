extends Node3D

func _ready():
	print("Debug terrain scene loaded")
	
	# Create terrain mesh instance using the C++ class
	var terrain_mesh = TerrainMesh.new()
	
	# Add it to the scene
	get_node("TerrainMesh").add_child(terrain_mesh)
	
	print("Terrain mesh added to scene")

func _input(event):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_1:
				# Reset camera to default view
				var camera = get_node("FixedCamera")
				camera.transform = Transform3D(1, 0, 0, 0, 0.707107, 0.707107, 0, -0.707107, 0.707107, 0, 15, 15)
				print("Camera reset to default view")
			KEY_2:
				# Move camera closer
				var camera = get_node("FixedCamera")
				camera.transform.origin += Vector3(0, -2, -2)
				print("Camera moved closer")
			KEY_3:
				# Move camera further
				var camera = get_node("FixedCamera")
				camera.transform.origin += Vector3(0, 2, 2)
				print("Camera moved further")
			KEY_4:
				# Rotate camera around terrain
				var camera = get_node("FixedCamera")
				camera.rotate_y(PI/4)
				print("Camera rotated 45 degrees") 