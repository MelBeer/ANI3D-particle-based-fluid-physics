#include "simulation.hpp"

using namespace cgp;

#define NB_FACE 6

#define ALPHA 0.8f
#define BETA 0.8f
#define MU 0.2f
#define EPSILON 0.001f

void simulate(std::vector<particle_structure>& particles, 
              cgp::grid_3D<std::vector<int>> hash_grid,
              std::vector<cgp::vec3> faces,
              std::vector<cgp::vec3> normals,
              float dt)
{
	vec3 const g = { 0,0,-9.81f };

	for (size_t kx = 0; kx < hash_grid.dimension.x; kx++) {
		for (size_t ky = 0; ky < hash_grid.dimension.y; ky++) {
			for (size_t kz = 0; kz < hash_grid.dimension.z; kz++) {
				
				std::vector<int> &particles_indices = hash_grid(kx, ky, kz);

				// For each particle in this cell kx,ky,kz
				for (size_t pi = 0; pi < particles_indices.size(); pi++)
				{
					int k = particles_indices[pi];
					
					particle_structure& part = particles[k];

					vec3 const f = part.m * g;

					// For every other Particle (all but k)
					for (size_t pj = 0; pj < particles_indices.size(); pj++)
					{
						int j = particles_indices[pj];
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

				particles_indices.clear();
			}
		}
	}

	for (size_t i = 0; i < particles.size(); i++)
	{
		particle_structure part = particles[i];

		int n_kx = (part.p.x + 1) * hash_grid.dimension.x / 2;
		int n_ky = (part.p.y + 1) * hash_grid.dimension.y / 2;
		int n_kz = (part.p.z + 1) * hash_grid.dimension.z / 2;

		hash_grid(n_kx, n_ky, n_kz).push_back(i);
	}
}
