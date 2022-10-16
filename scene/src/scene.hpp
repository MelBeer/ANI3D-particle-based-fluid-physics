#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "implicit_surface/implicit_surface.hpp"

using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = false;
	bool add_sphere = true;
};

// The structure of the custom scene
struct scene_structure : scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_event_periodic timer;
	float isovalue = 0.2f;
	implicit_surface_structure implicit_surface; // Structures used for the implicit surface (*)
	field_function_structure field_function;     // A Parametric function used to generate the discrete field (*)
	cgp::curve_drawable cube_wireframe;
	std::vector<cgp::vec3> cube_faces;
	std::vector<cgp::vec3> cube_normals;

	cgp::vec2 mouse_pos_current;

	// ****************************** //
	// Functions
	// ****************************** //

	void init_border_cube(int s);
	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void emit_particle();
	void simulation_step(float dt);
};





