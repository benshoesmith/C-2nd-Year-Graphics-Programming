#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoords;

out vec3 varying_normal;
out vec3 varying_position;
out vec2 varying_texcoords;

void main(void)
{
	varying_texcoords = vertex_texcoords;
	varying_normal = normalize(mat3(model_xform) * vertex_normal);
	varying_position = mat4x3(model_xform) * vec4(vertex_position, 1.0);
	gl_Position = combined_xform * vec4(vertex_position, 1.0);
}
