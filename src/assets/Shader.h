#pragma once
#include <glad/glad.h>
#include <string>
#include <util/Logger.h>

#include <glm/gtc/type_ptr.hpp>
#include <cpptoml.h>

// Not only do we load the shader, and make it easily usable
// we also run a preprocessor to allow includes
class Shader
{
private:


	// For the preprocessor
	std::string pkg;
	std::string name;
	
	// Run basic preprocessing on shader
	// We allow:
	// #include <absolute_path>		(Allows packages)
	// #include "relative_path"		(Doesn't allow packages, just relative paths)
	// (#define is already done by the GLSL compiler)
	// There MUST be an space after the macro name (#include<test> is INVALID) 
	// Relative paths are very strict, always use ../ for going back, etc...
	std::string preprocessor(const std::string& file);



public:

	GLuint id;

	void use();

	inline void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}
	inline void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	inline void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	inline void setVec2(const std::string& name, glm::vec2 value) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
	}

	inline void setVec3(const std::string& name, glm::vec3 value) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
	}

	inline void setVec4(const std::string& name, glm::vec4 value) const
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
	}

	inline void setMat4(const std::string& name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void setMat3(const std::string& name, glm::mat3 value) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	Shader(const std::string& vertexData, const std::string& fragmentData, const std::string& pkg, const std::string& path);
	~Shader();
};

Shader* load_shader(const std::string& path, const std::string& name, const std::string& pkg, const cpptoml::table& cfg);


