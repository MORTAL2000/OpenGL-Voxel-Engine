#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "GLErrorManager.h"

Shader::Shader(const std::string& filepath) 
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
				//Set to vertex
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
				//Set to fragment
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	GLCall(int id = glCreateShader(type));
	const char* src = source.c_str();

	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result = 0;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length = 0;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)malloc(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1iv(const std::string& name, signed int count, const int* value)
{
	GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform2i(const std::string& name, int v0, int v1)
{
	GLCall(glUniform2i(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3i(const std::string& name, int v0, int v1, int v2)
{
	GLCall(glUniform3i(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4i(const std::string& name, int v0, int v1, int v2, int v3)
{
	GLCall(glUniform4i(GetUniformLocation(name), v0, v1, v2, v3));
}


void Shader::SetUniform3iv(const std::string& name, signed int count, const int* value)
{
	GLCall(glUniform3iv(GetUniformLocation(name), count, value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}
