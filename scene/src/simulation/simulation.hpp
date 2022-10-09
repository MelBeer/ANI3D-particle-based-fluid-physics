#pragma once

#include "cgp/cgp.hpp"

struct particle_structure
{
    cgp::vec3 p; // Position
    cgp::vec3 v; // Speed

    cgp::vec3 c; // Color
    float r;     // Radius
    float m;     // Magnitude
};

void simulate(std::vector<particle_structure>& particles, 
              cgp::grid_3D<std::vector<int>> hash_grid,
              std::vector<cgp::vec3> faces,
              std::vector<cgp::vec3> normals,
              float dt);

