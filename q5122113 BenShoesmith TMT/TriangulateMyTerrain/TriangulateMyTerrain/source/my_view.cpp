#include "my_view.hpp"
#include <scene\types.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const scene::Context * scene)
{
	scene_ = scene;
}

void MyView::toggleShading()
{
	shade_normals_ = !shade_normals_;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
	assert(scene_ != nullptr);

	GLint compile_status = 0;
	GLint link_status = 0;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string
		= tygra::createStringFromFile("resource:///terrain_vs.glsl");
	const char *vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(vertex_shader, 1,
		(const GLchar **)&vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string
		= tygra::createStringFromFile("resource:///terrain_fs.glsl");
	const char *fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(fragment_shader, 1,
		(const GLchar **)&fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	terrain_sp_ = glCreateProgram();
	glAttachShader(terrain_sp_, vertex_shader);
	glDeleteShader(vertex_shader);
	glAttachShader(terrain_sp_, fragment_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(terrain_sp_);

	glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}


	// X and -Z are on the ground, Y is up
	const float sizeX = scene_->getTerrainSizeX();
	const float sizeY = scene_->getTerrainSizeY();
	const float sizeZ = scene_->getTerrainSizeZ();


	const size_t number_of_patches = scene_->getTerrainPatchCount();
	// below is an example of accessing a control point from the second patch
	scene::Vector3 cp = scene_->getTerrainPatchPoint(1, 2, 3);


	tygra::Image displace_image =
		tygra::createImageFromPngFile(scene_->getTerrainDisplacementMapName());

	// below is an example of reading the red component of pixel(x,y) as a byte [0,255]
	uint8_t displacement = *(uint8_t*)displace_image.pixel(1, 2);


	// below is placeholder code to create a tessellated quad
	// replace the hardcoded values with algorithms to create a tessellated quad

	/* std::vector<glm::vec3> positions = { { 0, 0, 0 }, { sizeX, 0, 0 },
	{sizeX, 0, -sizeZ}, { 0, 0, -sizeZ} };*/
	GridData grid;

	generateGrid(grid, 256, sizeX, sizeZ);

	applyBezierPatches(grid, scene_);
	recalcNormals(grid);
	applyDisplacement(grid, displace_image);
	recalcNormals(grid);
	applyNoise(grid);
	recalcNormals(grid);

	// below is indicative code for initialising a terrain VAO.

	glGenBuffers(1, &terrain_mesh_.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		grid.elements.size() * sizeof(GLuint),
		grid.elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	terrain_mesh_.element_count = grid.elements.size();

	glGenBuffers(1, &terrain_mesh_.position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
	glBufferData(GL_ARRAY_BUFFER, grid.positions.size() * sizeof(glm::vec3),
		grid.positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &terrain_mesh_.normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, grid.normals.size() * sizeof(glm::vec3),
		grid.normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &terrain_mesh_.vao);
	glBindVertexArray(terrain_mesh_.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
	glEnableVertexAttribArray(kVertexPosition);
	glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glEnableVertexAttribArray(kVertexNormal);
	glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyView::windowViewDidReset(tygra::Window * window,
	int width,
	int height)
{
	glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	glDeleteProgram(terrain_sp_);
	glDeleteBuffers(1, &terrain_mesh_.position_vbo);
	glDeleteBuffers(1, &terrain_mesh_.normal_vbo);
	glDeleteBuffers(1, &terrain_mesh_.element_vbo);
	glDeleteVertexArrays(1, &terrain_mesh_.vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
	assert(scene_ != nullptr);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	const float aspect_ratio = viewport[2] / (float)viewport[3];

	const auto& camera = scene_->getCamera();
	glm::mat4 projection_xform = glm::perspective(
		glm::radians(camera.getVerticalFieldOfViewInDegrees()),
		aspect_ratio,
		camera.getNearPlaneDistance(),
		camera.getFarPlaneDistance());
	glm::vec3 camera_pos = (const glm::vec3&)camera.getPosition();
	glm::vec3 camera_at = camera_pos + (const glm::vec3&)camera.getDirection();
	glm::vec3 world_up{ 0, 1, 0 };
	glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

	glUseProgram(terrain_sp_);

	GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
	glUniform1i(shading_id, shade_normals_);


	/* TODO: you are free to modify any of the drawing code below */


	glm::mat4 world_xform = glm::mat4(1);
	glm::mat4 view_world_xform = view_xform * world_xform;

	GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
		"projection_xform");
	glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
		glm::value_ptr(projection_xform));

	GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
		"view_world_xform");
	glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
		glm::value_ptr(view_world_xform));

	if (terrain_mesh_.vao) {
		glBindVertexArray(terrain_mesh_.vao);
		glDrawElements(GL_TRIANGLES, terrain_mesh_.element_count,
			GL_UNSIGNED_INT, 0);
	}
}

glm::vec3 MyView::cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float u)
{
	return (1 - u) * (1 - u) * (1 - u) * p0
		+ 3 * u * (1 - u) * (1 - u) * p1
		+ 3 * u * u * (1 - u) * p2
		+ u * u * u * p3;
}


void MyView::applyBezierPatches(GridData & grid, const scene::Context * scene_)
{
	int number_of_patches = scene_->getTerrainPatchCount();
	for (size_t i = 0; i < grid.positions.size(); i++)
	{

		int patchIndex = std::floor(grid.uvs[i].x * number_of_patches);
		float patchX = std::fmod(grid.uvs[i].x * number_of_patches, 1);

		std::vector<glm::vec3> points;
		if (patchIndex >= number_of_patches)
		{
			patchX = 1;
			patchIndex = number_of_patches - 1;
		}
		for (size_t y = 0; y < 4; y++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				points.push_back(ToVec3(scene_->getTerrainPatchPoint(patchIndex, x, y)));
			}
		}

		grid.positions[i].y = cubic2D(points, patchX, grid.uvs[i].y).y;
	}
}

void MyView::recalcNormals(GridData & grid)
{
	for (int i = 0; i < grid.elements.size(); i += 3)
	{
		glm::vec3 a = grid.positions[grid.elements[i]];
		glm::vec3 b = grid.positions[grid.elements[i + 1]];
		glm::vec3 c = grid.positions[grid.elements[i + 2]];

		glm::vec3 tri0 = b - a;
		glm::vec3 tri1 = c - a;

		glm::vec3 normal = glm::cross(tri0, tri1);

		grid.normals[grid.elements[i]] += normal;
		grid.normals[grid.elements[i]] = glm::normalize(grid.normals[grid.elements[i]]);

		grid.normals[grid.elements[i + 1]] += normal;
		grid.normals[grid.elements[i + 1]] = glm::normalize(grid.normals[grid.elements[i + 1]]);
		grid.normals[grid.elements[i + 2]] += normal;
		grid.normals[grid.elements[i + 2]] = glm::normalize(grid.normals[grid.elements[i + 2]]);
	}
}

glm::vec3 MyView::ToVec3(const scene::Vector3 & v)
{
	return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 MyView::cubic2D(const std::vector<glm::vec3> points, float x, float y)
{
	glm::vec3 curve[4];
	for (int i = 0; i < 4; i++)
	{
		curve[i] = cubicBezier(
			points[0 + (i * 4)],
			points[1 + (i * 4)],
			points[2 + (i * 4)],
			points[3 + (i * 4)],
			x
		);
	}
	return cubicBezier(curve[0], curve[1], curve[2], curve[3], y);
}

float MyView::noise(int x, int y)
{
	int n = x + y * 47;
	n = (n >> 13) ^ n;
	int nn = (n * (n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}

float MyView::cosineLerp(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft))* 0.5f;
	return a * (1.0f - f) + b * f;
}

float MyView::kenPerlin(float xPos, float zPos)
{
	float s, t, u, v;
	s = noise((int)xPos, (int)zPos);
	t = noise((int)xPos + 1, (int)zPos);
	u = noise((int)xPos, (int)zPos + 1);
	v = noise((int)xPos + 1, (int)zPos + 1);
	float c1 = cosineLerp(s, t, xPos);
	float c2 = cosineLerp(u, v, xPos);

	return cosineLerp(c1, c2, zPos);
}


void MyView::applyDisplacement(GridData & grid, tygra::Image& displace_image)
{
	//int number_of_patches = scene_->getTerrainPatchCount();
	for (int i = 0; i < grid.positions.size(); i++)
	{
		auto& p = grid.positions[i];
		auto& n = grid.normals[i];
		auto& t = grid.uvs[i];
		const int x = int(t.x * (displace_image.width() - 1));
		const int y = int(t.y * (displace_image.height() - 1));
		uint8_t height = *(uint8_t*)displace_image.pixel(x, y);
		p += n * (float)height;
	}
}

void MyView::applyNoise(GridData & grid)
{
	for (int i = 0; i < grid.positions.size(); i++)
	{
		float p = grid.positions[i].x;
		float q = grid.positions[i].z;
		grid.positions[i].y += kenPerlin(p, q) * 20;
	}
}

void MyView::generateGrid(GridData & grid, int resolution, int sizeX, int sizeZ)
{
	grid.resolution = resolution;
	grid.gSizeX = sizeX / grid.resolution;
	grid.gSizeZ = sizeZ / grid.resolution;
	grid.sizeX = sizeX;
	grid.sizeZ = sizeZ;
	bool even = true;
	for (int z = 0; z < grid.resolution + 1; z++)
	{
		for (int x = 0; x < grid.resolution + 1; x++)
		{
			grid.positions.push_back(glm::vec3(x * grid.gSizeX, 0, -z * grid.gSizeZ));
			grid.normals.push_back(glm::vec3(0, 1, 0));
			grid.uvs.push_back(glm::vec2(((float)x * (float)grid.gSizeX) / (float)sizeX, ((float)z * (float)grid.gSizeZ) / (float)sizeZ));

			even = !even;

			if (x < grid.resolution && z < grid.resolution)
			{
				if (even)
				{
					grid.elements.push_back((z * (grid.resolution + 1)) + x);
					grid.elements.push_back((z * (grid.resolution + 1)) + x + 1);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x + 1);

					grid.elements.push_back((z * (grid.resolution + 1)) + x);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x + 1);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x);
				}
				else
				{
					grid.elements.push_back((z * (grid.resolution + 1)) + x);
					grid.elements.push_back((z * (grid.resolution + 1)) + x + 1);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x);

					grid.elements.push_back((z * (grid.resolution + 1)) + x + 1);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x + 1);
					grid.elements.push_back(((z + 1) * (grid.resolution + 1)) + x);
				}

			}
		}
	};
}

