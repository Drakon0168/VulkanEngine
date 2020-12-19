#pragma once

#include "pch.h"

class Shader
{
private:
	std::string fragmentPath;
	std::string vertexPath;

public:

#pragma region Constructor

	Shader(std::string vertexPath = "shaders/frag.spv", std::string fragmentPath = "shaders/vert.spv");

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the path to the spir-v file for the fragment shader
	/// </summary>
	/// <returns>The path to the spir-v fragment shader file</returns>
	std::string GetFragmentPath();

	/// <summary>
	/// Sets the path to the shader file to be used for the fragment shader
	/// </summary>
	/// <param name="value">The value to set the fragment shader path to</param>
	void SetFragmentPath(std::string value);

	/// <summary>
	/// Returns the path to the spir-v file for the vertex shader
	/// </summary>
	/// <returns>The path to the spir-v vertex shader file</returns>
	std::string GetVertexPath();

	/// <summary>
	/// Sets the path to the shader file to be used for the vertex shader
	/// </summary>
	/// <param name="value">The value to set the vertex shader path to</param>
	void SetVertexPath(std::string value);

#pragma endregion
};