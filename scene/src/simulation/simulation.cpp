#include "simulation.hpp"

using namespace cgp;

#define NB_FACE 6

#define ALPHA 0.8f
#define BETA 0.8f
#define MU 0.2f
#define EPSILON 0.001f

void simulate(std::vector<particle_structure>& particles, std::vector<vec3> faces, std::vector<vec3> normals, float dt)
{
	vec3 const g = { 0,0,-9.81f };
	size_t const N = particles.size();
	
	// For each Particle
	for (size_t k = 0; k < N; ++k)
	{
		particle_structure& part = particles[k];

		vec3 const f = part.m * g;

		// For every other Particle (all but k)
		for (size_t j = 0; j < N; j++)
		{
			if (j == k) continue;

			particle_structure& part_1 = particles[j];
			// If they collide
			if (norm(part.p - part_1.p) <= (part.r + part_1.r) + EPSILON)
			{
				vec3 u = (part.p - part_1.p) / norm(part.p - part_1.p);

				vec3 v_ortho = dot(part.v, u) * u;
				vec3 v_par = part.v - v_ortho;
				vec3 v1_ortho = dot(part_1.v, u) * u;
				vec3 v1_par = part_1.v - v1_ortho;

				if (norm(part.v) > EPSILON)
					part.v = ALPHA * v_par + BETA * v1_ortho; // += dot(part_1.v - part.v, u) * u;
				else
					part.v = MU * part.v;

				if (norm(part_1.v) > EPSILON)
					part_1.v = ALPHA * v1_par + BETA * v_ortho; // -= dot(part_1.v - part.v, u) * u;
				else
					part_1.v = MU * part_1.v;

				float d = part.r + part_1.r + EPSILON - norm(part.p - part_1.p);
				part.p += d/2 * u;
				part_1.p -= d/2 * u;
			}
		}
		

		for (size_t i = 0; i < NB_FACE; i++)
		{
			double dist = dot(part.p - faces[i], normals[i]);
			if (dist <= part.r)
			{
				part.p += (part.r - dist) * normals[i]; 
				part.v = part.v - 2 * dot(part.v, normals[i]) * normals[i];
			}
		}

		part.v = (1 - 0.9f * dt) * part.v + dt * f;
		part.p = part.p + dt * part.v;
	}
}
