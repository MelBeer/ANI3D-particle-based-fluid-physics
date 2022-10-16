#pragma once

#include "cgp/cgp.hpp"

struct particle_structure
{
    cgp::vec3 p; // Position
    cgp::vec3 v; // Speed

    float r;     // Radius
    float m;     // Magnitude
    float rho;   // Density
};

const float h = 1.0f;     // Kernel radius
const float s = 1.0f;     // Stiffness
const float rho_0 = 1.0f; // Rest density
const float nu = 0.01f;   // Viscosity

std::vector<cgp::int3> simulate(std::vector<particle_structure>& particles, 
                           cgp::grid_3D<std::vector<int>> &hash_grid,
                           std::vector<cgp::vec3> faces,
                           std::vector<cgp::vec3> normals,
                           float dt);

