#include "field_function.hpp"

#define EPSILON 0.001f

using namespace cgp;

// Parameterization Gaussian centered at point p0
static float gaussian(vec3 const& p, vec3 const& p0, float sigma)
{
	float const d = norm(p - p0);
	float const value = std::exp(-(d * d) / (sigma * sigma));
	return value;
}

float field_function_structure::operator()(cgp::vec3 const& p) const
{
	float value = 0.0f;
	
	int kx = (p.x + 1 - EPSILON) * hash_grid.dimension.x / 2;
	int ky = (p.y + 1 - EPSILON) * hash_grid.dimension.y / 2;
	int kz = (p.z + 1 - EPSILON) * hash_grid.dimension.z / 2;

	// For each neighbouring cell
	for (int off_x = -1; off_x < 2; off_x++) {
		for (int off_y = -1; off_y < 2; off_y++) {
			for (int off_z = -1; off_z < 2; off_z++) {

				if (kx + off_x < 0 || kx + off_x > hash_grid.dimension.x - 1
					|| ky + off_y < 0 || ky + off_y > hash_grid.dimension.y - 1
					|| kz + off_z < 0 || kz + off_z > hash_grid.dimension.z - 1)
					continue;
								
				std::vector<int> const &neighbour_particles_indices = hash_grid(kx + off_x, ky + off_y, kz + off_z);

				for (size_t pi = 0; pi < neighbour_particles_indices.size(); pi++)
				{
					size_t i = neighbour_particles_indices[pi];

					particle_structure pa = particles[i];
					value += pa.m * gaussian(p, pa.p, pa.r);
				}
			}
		}
	}

	if (noise_magnitude > 0) {
		vec3 const offset = vec3{ noise_offset + 1000, 1000, 1000 };
		vec3 const p_noise = noise_scale * p + offset;
		value += noise_magnitude * noise_perlin(p_noise, noise_octave, noise_persistance);
	}

	return value;
}
