#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Framebuffer
{
private:
	size_t width, height;
	
	GLint unbind_to;

	GLsizei unbind_viewport[4];

public:

	// GL framebuffer object
	GLuint fbuffer;
	// The texture color buffer you can actually read
	// -> Use this with glBindTexture to use it as a texture <-
	GLuint tex_color_buffer;
	// The renderbuffer used internally to store depth and stencil
	GLuint rbo;

	// Binds the framebuffer for rendering to it
	void bind();

	// Unbinds the framebuffer
	void unbind();

	void set_viewport();

	glm::ivec2 get_size();

	Framebuffer(size_t width, size_t height, GLuint rbo_override = 0);
	~Framebuffer();
};


