#pragma once
#include <tygra/FileHelper.hpp>
#include <scene\types.hpp>
#include <scene/context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

class MyView : public tygra::WindowViewDelegate
{
public:

	MyView();

	~MyView();

	void setScene(const scene::Context * scene);

	void toggleShading();

private:

	void windowViewWillStart(tygra::Window * window) override;

	void windowViewDidReset(tygra::Window * window,
		int width,
		int height) override;

	void windowViewDidStop(tygra::Window * window) override;

	void windowViewRender(tygra::Window * window) override;

	glm::vec3 cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float u);


private:

	const scene::Context * scene_{ nullptr };

	GLuint terrain_sp_{ 0 };

	bool shade_normals_{ false };

	struct MeshGL
	{
		GLuint position_vbo{ 0 };
		GLuint normal_vbo{ 0 };
		GLuint element_vbo{ 0 };
		GLuint vao{ 0 };
		int element_count{ 0 };
	};
	struct GridData
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<GLuint> elements;
		std::vector<glm::vec2> uvs;
		int resolution;
		int gSizeX;
		int gSizeZ;
		int sizeX;
		int sizeZ;
	};

	void applyBezierPatches(GridData& grid, const scene::Context * scene_);

	void recalcNormals(GridData &grid);

	glm::vec3 ToVec3(const scene::Vector3& v);

	glm::vec3 cubic2D(const std::vector<glm::vec3> points, float x, float y);

	void generateGrid(GridData& grid, int resolution, int sizeX, int sizeZ);

	float noise(int x, int y);

	float cosineLerp(float a, float b, float x);

	float kenPerlin(float xPos, float zPos);


	void applyNoise(GridData& grid);

	MeshGL terrain_mesh_;

	void applyDisplacement(GridData& grid, tygra::Image& displace_image);

	enum
	{
		kVertexPosition = 0,
		kVertexNormal = 1,
	};

};
