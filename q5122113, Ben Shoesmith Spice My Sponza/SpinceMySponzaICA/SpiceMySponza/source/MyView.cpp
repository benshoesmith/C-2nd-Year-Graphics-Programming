#include "MyView.hpp"
#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const sponza::Context * scene)
{
	scene_ = scene;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
	assert(scene_ != nullptr);
	GLint compile_status = GL_FALSE;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string
		= tygra::createStringFromFile("resource:///sponza_vs.glsl");
	const char * vertex_shader_code = vertex_shader_string.c_str();
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
		= tygra::createStringFromFile("resource:///sponza_fs.glsl");
	const char * fragment_shader_code = fragment_shader_string.c_str();
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

	// program object made to host shaders and their settings
	SpiceMySponza = glCreateProgram();
	glAttachShader(SpiceMySponza, vertex_shader);
	glBindAttribLocation(SpiceMySponza, kVertexPosition, "vertex_position");
	glBindAttribLocation(SpiceMySponza, kVertexNormal, "vertex_normal");
	glBindAttribLocation(SpiceMySponza, kTextureTest, "vertex_texcoords");
	glDeleteShader(vertex_shader);
	glAttachShader(SpiceMySponza, fragment_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(SpiceMySponza);

	GLint link_status = GL_FALSE;
	glGetProgramiv(SpiceMySponza, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(SpiceMySponza, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	sponza::GeometryBuilder builder;

	const auto& scene_meshes = builder.getAllMeshes();
	for (const auto& scene_mesh : scene_meshes)
	{
		MeshGL& newMesh = meshes_[scene_mesh.getId()];

		const auto& positions = scene_mesh.getPositionArray();
		const auto& elements = scene_mesh.getElementArray();
		const auto& normal = scene_mesh.getNormalArray();
		const auto& texcoords = scene_mesh.getTextureCoordinateArray();

		// creating opengl buffers
		glGenBuffers(1, &newMesh.position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.position_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			positions.size() * sizeof(glm::vec3), // size of data in bytes
			positions.data(), // pointer to the data
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		glGenBuffers(1, &newMesh.normal_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			normal.size() * sizeof(glm::vec3),
			normal.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		glGenBuffers(1, &newMesh.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			elements.size() * sizeof(unsigned int),
			elements.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kNullId);
		newMesh.element_count = elements.size();

		glGenVertexArrays(1, &newMesh.vao);
		glBindVertexArray(newMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);

		glGenBuffers(1, &newMesh.texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.texcoords);
		glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);
		//small texture - are buffers populated incorrectly?

		glBindBuffer(GL_ARRAY_BUFFER, newMesh.position_vbo);
		glEnableVertexAttribArray(kVertexPosition);
		glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_vbo);
		glEnableVertexAttribArray(kVertexNormal);
		glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, kNullId);
		glBindVertexArray(kNullId);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	tygra::Image texture_image
		= tygra::createImageFromPngFile("resource:///diff0.png");
	if (texture_image.doesContainData()) {
		glGenTextures(1, &sponza_texture_);
		glBindTexture(GL_TEXTURE_2D, sponza_texture_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1
			? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
			texture_image.pixelData());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, kNullId);
	}
}

void MyView::windowViewDidReset(tygra::Window * window,
	int width,
	int height)
{
	glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	glDeleteProgram(SpiceMySponza);
	glDeleteBuffers(1, &mesh_.position_vbo);
	glDeleteBuffers(1, &mesh_.element_vbo);
	glDeleteVertexArrays(1, &mesh_.vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	assert(scene_ != nullptr);

	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, sponza_texture_);
	glUniform1i(glGetUniformLocation(SpiceMySponza, "sponzaSampler"), 0);

	const auto& Scenelights = scene_->getAllLights();

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio
		= viewport_size[2] / (float)viewport_size[3];
	//Configure OpenGL Pipeline Settings
	glm::vec3 camera_position = (const glm::vec3 &)scene_->getCamera().getPosition();
	glm::mat4 projection_xform = glm::perspective(glm::radians(75.f),
		aspect_ratio,
		1.f, 1000.f);
	glm::mat4 view_xform = glm::lookAt((const glm::vec3&)scene_->getCamera().getPosition(),
		(const glm::vec3&)scene_->getCamera().getPosition() + (const glm::vec3&)scene_->getCamera().getDirection(),
		glm::vec3(0, 1, 0));

	// struct for the lights in fragment shader
	struct Light
	{
		glm::vec3 position;
		float range;
		glm::vec3 intensity;
	};

	GLuint noLights_id = glGetUniformLocation(SpiceMySponza, "noLights");
	glUniform1i(noLights_id, Scenelights.size());

	// Loops through lights to give them  struct values
	for (int i = 0; i < Scenelights.size(); i++)
	{
		GLuint lights_id = glGetUniformLocation(SpiceMySponza, std::string("lights[" + std::to_string(i) + "].position").c_str());
		glUniform3fv(lights_id, 1, glm::value_ptr((const glm::vec3 &)Scenelights[i].getPosition()));

		lights_id = glGetUniformLocation(SpiceMySponza, std::string("lights[" + std::to_string(i) + "].intensity").c_str());
		glUniform3fv(lights_id, 1, glm::value_ptr((const glm::vec3 &)Scenelights[i].getIntensity()));

		lights_id = glGetUniformLocation(SpiceMySponza, std::string("lights[" + std::to_string(i) + "].range").c_str());
		glUniform1f(lights_id, Scenelights[i].getRange());
	}

	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Loop for code to draw the mesh using model xform
	for (const auto& instance : scene_->getAllInstances()) {
		glm::mat4 model_xform = glm::mat4((const glm::mat4x3 &)instance.getTransformationMatrix());
		const MeshGL& mesh = meshes_[instance.getMeshId()];

		glm::mat4 combined_xform = projection_xform
			* view_xform
			* model_xform;

		glUseProgram(SpiceMySponza);

		GLuint combined_xform_id = glGetUniformLocation(SpiceMySponza,
			"combined_xform");
		glUniformMatrix4fv(combined_xform_id,
			1, GL_FALSE, glm::value_ptr(combined_xform));
		GLuint model_xform_id = glGetUniformLocation(SpiceMySponza,
			"model_xform");
		glUniformMatrix4fv(model_xform_id,
			1, GL_FALSE, glm::value_ptr(model_xform));

		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, 0);
	}
}
