#pragma once

#include "cgp/cgp.hpp"

struct particle_structure
{
    cgp::vec3 p; // Position
    cgp::vec3 v; // Speed

    float r;     // Radius
    float h;     // Kernel Radius
    float m;     // Magnitude
};

std::vector<cgp::int3> simulate(std::vector<particle_structure>& particles, 
                           cgp::grid_3D<std::vector<int>> &hash_grid,
                           std::vector<cgp::vec3> faces,
                           std::vector<cgp::vec3> normals,
                           float dt);

