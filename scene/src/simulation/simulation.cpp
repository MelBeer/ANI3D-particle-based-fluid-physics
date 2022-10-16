#include "simulation.hpp"

using namespace cgp;

#define ALPHA 0.8f
#define BETA 0.8f
#define MU 0.2f
#define EPSILON 0.001f

template <class T>
int3 grid_indices_from_pos(grid_3D<T> grid, vec3 pos) {
	int kx = (pos.x + 1 - EPSILON) * grid.dimension.x / 2;
	int ky = (pos.y + 1 - EPSILON) * grid.dimension.y / 2;
	int kz = (pos.z + 1 - EPSILON) * grid.dimension.z / 2;

	if (kx < 0 || ky < 0 || kz < 0)
		std::cout << "HEeELP !! pos = " << pos << std::endl;

	return { kx, ky, kz };
};

std::vector<int3> simulate(std::vector<particle_structure>& particles, 
						   cgp::grid_3D<std::vector<int>> &hash_grid,
						   std::vector<cgp::vec3> faces,
						   std::vector<cgp::vec3> normals,
						   float dt)
{
	std::vector<int3> occupied_cells;
	vec3 const g = { 0,0,-9.81f };

	std::vector<bool> seen(particles.size(), false);
	
	// For each cell
	for (size_t kx = 0; kx < hash_grid.dimension.x; kx++) {
		for (size_t ky = 0; ky < hash_grid.dimension.y; ky++) {
			for (size_t kz = 0; kz < hash_grid.dimension.z; kz++) {

				std::vector<int> &particles_indices = hash_grid(kx, ky, kz);

				// For each particle in this cell kx,ky,kz
				for (size_t pi = 0; pi < particles_indices.size(); pi++)
				{
					int k = particles_indices[pi];

					if (seen[k]) continue;
					seen[k] = true;
					
					particle_structure& part = particles[k];

					vec3 const f = part.m * g;

					// For each neighbouring cell
					for (int off_x = -1; off_x < 2; off_x++) {
						for (int off_y = -1; off_y < 2; off_y++) {
							for (int off_z = -1; off_z < 2; off_z++) {

								if (kx + off_x < 0 || kx + off_x > hash_grid.dimension.x - 1
									|| ky + off_y < 0 || ky + off_y > hash_grid.dimension.y - 1
									|| kz + off_z < 0 || kz + off_z > hash_grid.dimension.z - 1)
									continue;
												
								std::vector<int> &neighbour_particles_indices = hash_grid(kx + off_x, ky + off_y, kz + off_z);

								// For every other Particle (all but k) in surrounding cells
								for (size_t pj = 0; pj < neighbour_particles_indices.size(); pj++)
								{
									int j = neighbour_particles_indices[pj];
									if (j == k) continue;

									particle_structure& part_1 = particles[j];
									if (part.p.x == part_1.p.x && part.p.y == part_1.p.y && part.p.z == part_1.p.z)
										part_1.p += 0.000001f;

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
							}
						}
					}
					
					for (size_t i = 0; i < faces.size(); i++)
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

					int3 grid_i = grid_indices_from_pos<>(hash_grid, part.p);

					if (grid_i.x != kx || grid_i.y != ky || grid_i.z != kz) {
						particles_indices.erase(particles_indices.begin() + pi);
						std::vector<int> &dest_cell = hash_grid(grid_i);
						dest_cell.push_back(k);

						if (grid_i.x <= kx && grid_i.y <= ky && grid_i.z <= kz && dest_cell.size() == 1) {
							occupied_cells.push_back({ kx,ky,kz });
						}
					}
				}

				if (particles_indices.size() > 0)
					occupied_cells.push_back({ kx,ky,kz });
			}
		}
	}

	return occupied_cells;
}
