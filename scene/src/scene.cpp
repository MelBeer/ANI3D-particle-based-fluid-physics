#include "scene.hpp"


using namespace cgp;

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 3.0f, 2.0f, 0.0f }, {0,0,0}, {0,0,1});
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	mouse_pos_current = inputs.mouse.position.current;

	timer.event_period = 0.5f;

	// Edges of the containing cube in [-1,1]^3
	//  Note: this data structure is set for display purpose - don't use it to compute some information on the cube - it would be un-necessarily complex
	numarray<vec3> border_cube = { {-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
		{-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
		{-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1} };

	cube_faces = { {1,0,0}, {0,1,0}, {0,0,1}, {-1,0,0}, {0,-1,0}, {0,0,-1} };
	cube_normals = { {-1,0,0}, {0,-1,0}, {0,0,-1}, {1,0,0}, {0,1,0}, {0,0,1} };
	cube_wireframe.initialize_data_on_gpu(border_cube);
	cube_wireframe.display_type = curve_drawable_display_type::Segments;

	sphere.initialize_data_on_gpu(mesh_primitive_sphere());
}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();

	mouse_pos_current = inputs.mouse.position.current;

	// Create a new particle if needed
	emit_particle();

	// Call the simulation of the particle system
	float const dt = 0.01f * timer.scale;
	if (inputs.mouse.click.left && inputs.keyboard.shift) {

		auto const& orientation = camera_control.camera_model.orientation();
		if (inputs.keyboard.ctrl) {
			vec3 translation = orientation * vec3(inputs.mouse.position.current - inputs.mouse.position.previous, 0);
			cube_wireframe.model.translation += translation;
			for (int i = 0; i < 6; i++) {
				cube_faces[i] = cube_faces[i] + translation;
			}
		}
		else {
			auto R = rotation_transform::from_vector_transform(
				normalize(orientation * vec3(inputs.mouse.position.previous, 0)),
				normalize(orientation * vec3(inputs.mouse.position.current, 0))
			);
			vec3& T = cube_wireframe.model.translation;
			cube_wireframe.model.rotation = R * cube_wireframe.model.rotation;
			for (int i = 0; i < 6; i++) {
				cube_faces[i] = R.matrix() * (cube_faces[i] - T) + T;
				cube_normals[i] = R.matrix() * cube_normals[i];
			}
		}
		
		inputs.mouse.position.update(inputs.mouse.position.current);
	}
	simulate(particles, cube_faces, cube_normals, dt);

	// Display the result
	sphere_display();

	if (gui.display_frame)
		draw(global_frame, environment);
}

void scene_structure::sphere_display()
{
	// Display the particles as spheres
	size_t const N = particles.size();
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure const& particle = particles[k];
		sphere.material.color = particle.c;
		sphere.model.translation = particle.p;
		sphere.model.scaling = particle.r;

		draw(sphere, environment);
	}

	// Display the box in which the particles should stay
	draw(cube_wireframe, environment);
}

void scene_structure::emit_particle()
{
	// Emit particle with random velocity
	//  Assume first that all particles have the same radius and mass
	static numarray<vec3> const color_lut = { {1,0,0},{0,1,0},{0,0,1},{1,1,0},{1,0,1},{0,1,1} };
	if (timer.event && gui.add_sphere) {
		float const theta = rand_interval(0, 2 * Pi);
		vec3 const v = vec3(1.0f * std::cos(theta), 1.0f * std::sin(theta), 4.0f);

		particle_structure particle;
		particle.p = { 0,0,0 };
		particle.r = 0.08f;
		particle.c = {0.16,0.56,0.96}; // color_lut[int(rand_interval() * color_lut.size())];
		particle.v = v;
		particle.m = 1.0f; //

		particles.push_back(particle);
	}
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::SliderFloat("Time scale", &timer.scale, 0.05f, 2.0f, "%.2f s");
	ImGui::SliderFloat("Time to add new sphere", &timer.event_period, 0.05f, 2.0f, "%.2f s");
	ImGui::Checkbox("Add sphere", &gui.add_sphere);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

