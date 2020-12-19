#include "pch.h"
#include "Shader.h"

#pragma region Constructor

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	this->fragmentPath = fragmentPath;
	this->vertexPath = vertexPath;
}

#pragma endregion

#pragma region Accessors

std::string Shader::GetFragmentPath()
{
	return fragmentPath;
}

void Shader::SetFragmentPath(std::string value)
{
	fragmentPath = value;
}

std::string Shader::GetVertexPath()
{
	return vertexPath;
}

void Shader::SetVertexPath(std::string value)
{
	vertexPath = value;
}

#pragma endregion