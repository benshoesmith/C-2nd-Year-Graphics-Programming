#version 330

struct Light 
{
	vec3 position;
	float range;
	vec3 intensity;
};
// 22 Lights in scene
uniform Light lights[22];
uniform sampler2D sponzaSampler;

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texcoords;

out vec4 fragment_colour;

// Lambert function returns total of Lambert equation
vec3 Lambert(vec3 diffuse, float amb_intensity, vec3 amb_colour, vec3 diffuse_colour, vec3 light_colour)
{
	return(amb_intensity * amb_colour + diffuse_colour * diffuse * light_colour);
}

// Attenuation equation calcs distance to light and returns total of attenuation equation
vec3 Attenuation(vec3 light_position, vec3 P, float range, vec3 light_colour)
{
	float distance_to_light = distance(light_position, P);
	return light_colour * (1.0 - smoothstep(0, range, distance_to_light));
}

void main(void)
{
	vec3 tex_colour = texture(sponzaSampler, varying_texcoords).rgb;
	float amb_intensity = 0.2;
	vec3 amb_colour = vec3(tex_colour);
	vec3 diffuse_colour = vec3(tex_colour);

	vec3 light_colour = vec3(1.0);

	vec3 light_position = vec3(lights[0].position);
	vec3 P = varying_position;
	vec3 L = normalize(light_position - P);
	vec3 N = normalize(varying_normal);
	float diffuse = clamp(dot(L, N), 0.0, 1.0);
	vec3 col = vec3(diffuse);

	

	vec3 light_dot_product = vec3(0.0);
	// Loops thorugh lights
	for (int i = 0; i < 22; i++)
	{
		light_dot_product += Lambert(col, amb_intensity, amb_colour, diffuse_colour, light_colour) * Attenuation(lights[i].position, P, lights[i].range, light_colour);
	}

	fragment_colour = vec4(light_dot_product, 1.0);
}