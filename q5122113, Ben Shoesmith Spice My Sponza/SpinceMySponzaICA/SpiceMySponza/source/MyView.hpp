#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <chrono>

#include <vector>
#include <memory>
#include <map>

class MyView : public tygra::WindowViewDelegate
{
public:

	MyView();

	~MyView();

	void setScene(const sponza::Context * scene);

private:

	void windowViewWillStart(tygra::Window * window) override;

	void windowViewDidReset(tygra::Window * window,
		int width,
		int height) override;

	void windowViewDidStop(tygra::Window * window) override;

	void windowViewRender(tygra::Window * window) override;

private:

	std::chrono::system_clock::time_point start_time_;

	const sponza::Context * scene_;

	struct MeshGL
	{
		// VertexBufferObject for the vertex positions
		GLuint position_vbo{ 0 };

		GLuint normal_vbo{ 0 };

		// VertexBufferObject for the elements (indices)
		GLuint element_vbo{ 0 };

		//VertexBufferObject for the texture
		GLuint texcoords{ 0 };

		// VertexArrayObject for the shape's vertex array settings
		GLuint vao{ 0 };

		// Needed for when we draw using the vertex arrays
		int element_count{ 0 };

		GLuint colour_vbo{ 0 };
	};

	MeshGL mesh_;

	std::map<sponza::MeshId, MeshGL> meshes_;

	GLuint SpiceMySponza{ 0 };
	GLuint sponza_texture_{ 0 };
	const static GLuint kNullId = 0;
	enum VertexAttribIndexes {
		kVertexPosition = 0,
		kVertexColour = 1,
		kVertexNormal = 2
	};
	enum FragmentDataIndexes {
		kFragmentColour = 0
	};
	enum TextureIndexes {
		kTextureTest = 0
	};


};
