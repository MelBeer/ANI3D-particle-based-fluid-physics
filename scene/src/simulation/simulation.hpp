#pragma once

#include "cgp/cgp.hpp"

struct particle_structure
{
    cgp::vec3 p; // Position
    cgp::vec3 v; // Speed

    cgp::vec3 c; // Color
    float r;     // Radius
    float m;     // mass
};

void simulate(std::vector<particle_structure>& particles, std::vector<cgp::vec3> faces, std::vector<cgp::vec3> normals, float dt);

