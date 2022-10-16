#include "simulation.hpp"

using namespace cgp;

#define ALPHA 0.8f
#define BETA 0.8f
#define MU 0.2f
#define EPSILON 0.000001f

template <class T>
int3 grid_indices_from_pos(grid_3D<T> grid, vec3 pos) {
	int kx = (pos.x + 1 - EPSILON) * grid.dimension.x / 2;
	int ky = (pos.y + 1 - EPSILON) * grid.dimension.y / 2;
	int kz = (pos.z + 1 - EPSILON) * grid.dimension.z / 2;

	bool debug = false;
	if (kx < 0) { kx = 0; debug = true; }
	if (ky < 0) { ky = 0; debug = true; }
	if (kz < 0) { kz = 0; debug = true; }
	if (kx >= grid.dimension.x) { kx = grid.dimension.x - 1; debug = true; }
	if (ky >= grid.dimension.y) { ky = grid.dimension.y - 1; debug = true; }
	if (kz >= grid.dimension.z) { kz = grid.dimension.z - 1; debug = true; }

	if (debug)
		std::cout << "HEeELP !! pos = " << pos << std::endl;

	return { kx, ky, kz };
};

const float h9 = pow(h, 9.);
const float h6 = pow(h, 6.);

float Wh_poly_6(float d) {
	if (d < 0 || d > h)
		return 0.;
	return 315. * pow(h*h - d*d, 3.) / (64.*M_PI*h9);
}

float nabla_Wh_spiky(float d) {
	if (d < 0 || d > h)
		return 0.;
	return -45. * pow(h - d, 2.) / (M_PI * h6);
}

float triangle_Wh_spiky(float d) {
	if (d < 0 || d > h)
		return 0.;
	return 45. * (h - d) / (M_PI * h6);
}

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

					float rho = 0.0f;
					const float pressure = s * (part.rho - rho_0);

					vec3 const F_weight = part.m * g;
					vec3 F_pressure = vec3(0,0,0);
					vec3 F_viscosity = vec3(0,0,0);

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

									particle_structure& part_j = particles[j];
									
									//std::cout << "v : " << part.v << " | p : " << part.p << std::endl;
									vec3 dir = part.p - part_j.p;
									float dist = norm(dir);
									if (dist == 0.)
										dist = EPSILON;
									dir /= dist;

									rho += part_j.m * Wh_poly_6(dist);
									const float pressure_j = s * (part_j.rho - rho_0);
									
									F_pressure += part_j.m * ((pressure + pressure_j) / (2*part_j.rho)) * nabla_Wh_spiky(dist) * dir;
									F_viscosity += part_j.m * ((part_j.v - part.v) / part_j.rho) * triangle_Wh_spiky(dist) * dir;
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
							//part.v = part.v - 2 * dot(part.v, normals[i]) * normals[i];
							vec3 f_wall = 1.4 * normals[i] * dot(-normals[i], part.v);
							part.v += f_wall;
						}
					}

					/*if (rho > 0 && rho < 1)
						std::cout << "rho : " << rho << std::endl;*/

					F_pressure *= -part.m / part.rho;
					F_viscosity *= part.m * nu;

					part.rho = rho > 0 ? rho : part.rho;

					part.v += dt * (F_weight + F_pressure + F_viscosity) / part.m; // (1 - 0.9f * dt) * part.v + dt * f;
					part.p = part.p + dt * part.v;

					//std::cout << "v : " << part.v << " | p : " << part.p << std::endl;
					//std::cout << "nb p : " << particles.size() << std::endl;
					int3 grid_i = grid_indices_from_pos<>(hash_grid, part.p);

					if (grid_i.x != kx || grid_i.y != ky || grid_i.z != kz) {
						particles_indices.erase(particles_indices.begin() + pi);
						std::vector<int> &dest_cell = hash_grid(grid_i);
						dest_cell.push_back(k);

						if (grid_i.x <= kx && grid_i.y <= ky && grid_i.z <= kz && dest_cell.size() == 1) {
							occupied_cells.push_back({ kx,ky,kz });
						}
					}

					//std::cout << "\nNext Frame !\n" << std::endl;
				}

				if (particles_indices.size() > 0)
					occupied_cells.push_back({ kx,ky,kz });
			}
		}
	}

	return occupied_cells;
}
