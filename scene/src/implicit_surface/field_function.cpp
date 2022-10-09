#include "field_function.hpp"

using namespace cgp;

// Parameterization Gaussian centered at point p0
static float gaussian(vec3 const& p, particle_structure const& pa, float sigma)
{
	float const d = norm(p - pa.p);
	float const value = std::exp(-(d * d) / (sigma * sigma));
	return value;
}

float field_function_structure::operator()(cgp::vec3 const& p) const
{
	float value = 0.0f;
	for (size_t i = 0; i < particles.size(); i++)
	{
		particle_structure pa = particles[i];
		value += pa.m * gaussian(p, pa, pa.r);
	}

	// std::cout << (value != 0 ? value : 0) << std::endl;

	if (noise_magnitude > 0) {
		vec3 const offset = vec3{ noise_offset + 1000, 1000, 1000 };
		vec3 const p_noise = noise_scale * p + offset;
		value += noise_magnitude * noise_perlin(p_noise, noise_octave, noise_persistance);
	}

	return value;
}