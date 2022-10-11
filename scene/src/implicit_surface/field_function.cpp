#include "field_function.hpp"

using namespace cgp;

// Parameterization Gaussian centered at point p0
static float gaussian(vec3 const& p, vec3 const& p0, float h, float sigma)
{
	float const d = norm(p - p0);
	float value = 0.0f;
	if (d < h) {
		value = std::exp(-(d * d) / (sigma * sigma)); // 315.0f * pow(h * h - d * d, 3) / (64.0f * Pi * pow(h, 9)); 
	}
	return value;
}

float field_function_structure::operator()(cgp::vec3 const& p) const
{
	unsigned int kx = ((p.x + 1) * hash_grid.dimension.x - 0.01f) / 2;
	unsigned int ky = ((p.y + 1) * hash_grid.dimension.y - 0.01f) / 2;
	unsigned int kz = ((p.z + 1) * hash_grid.dimension.z - 0.01f) / 2;

	float value = 0.0f;
	for (size_t i = 0; i < particles.size(); i++)
	{
		particle_structure pa = particles[i];
		value += pa.m * gaussian(p, pa.p, pa.r * 1.2f, pa.r);
	}
	/*for (int kx_off = -1; kx_off < 2; kx_off++) {
		for (int ky_off = -1; ky_off < 2; ky_off++) {
			for (int kz_off = -1; kz_off < 2; kz_off++) {
				if (kx + kx_off < 0 || kx + kx_off > hash_grid.dimension.x -1
					|| ky + ky_off < 0 || ky + ky_off > hash_grid.dimension.y -1
					|| kz + kz_off < 0 || kz + kz_off > hash_grid.dimension.z -1)
				{
					continue;
				}

				std::vector<int> particle_indices = hash_grid(kx + kx_off, ky + ky_off, kz + kz_off); 

				for (size_t pi = 0; pi < particle_indices.size(); pi++)
				{
					int i = particle_indices[pi];
					particle_structure pa = particles[i];
					value += pa.m * gaussian(p, pa.p, pa.r * 2, pa.r);
				}
			}
		}
	}*/

	if (noise_magnitude > 0) {
		vec3 const offset = vec3{ noise_offset + 1000, 1000, 1000 };
		vec3 const p_noise = noise_scale * p + offset;
		value += noise_magnitude * noise_perlin(p_noise, noise_octave, noise_persistance);
	}

	return value;
}
