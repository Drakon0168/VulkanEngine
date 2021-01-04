#pragma once

#include "pch.h"

#include "Buffer.h"

//Forward declaration to get around circular dependency
class Mesh;

class Shader
{
private:
	std::string fragmentPath;
	std::string vertexPath;
	size_t textureCount;

	std::vector<std::shared_ptr<Buffer>> resources;

public:

#pragma region Constructor

	Shader(std::string vertexPath = "shaders/frag.spv", std::string fragmentPath = "shaders/vert.spv", size_t textureCount = 0, std::vector<std::shared_ptr<Buffer>> resources = std::vector<std::shared_ptr<Buffer>>());

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

	/// <summary>
	/// The number of textures expected by this shader
	/// </summary>
	/// <returns>The number of textures expected by this shader</returns>
	size_t GetTextureCount();

	/// <summary>
	/// Sets the number of textures that this shader expects
	/// </summary>
	void SetTextureCount(size_t value);

	/// <summary>
	/// Returns the list of buffer resources used by this shader
	/// </summary>
	/// <returns>The list of buffers used by the shader</returns>
	std::vector<std::shared_ptr<Buffer>> GetResources();

	/// <summary>
	/// Sets the resources that will be used by the shader, this list must be in the order that the shader expects the data to be in based on the vertex attribute description used by the material
	/// </summary>
	/// <param name="value">The list of buffers to set resources to</param>
	void SetResources(std::vector<std::shared_ptr<Buffer>> value);

#pragma endregion

#pragma region Bindings

	/// <summary>
	/// Binds shader resources to the command buffer for the specified mesh
	/// </summary>
	/// <param name="commandBuffer">The command buffer to bind to</param>
	/// <param name="mesh">The mesh to use for any mesh specific resources buffer</param>
	virtual void BindResources(VkCommandBuffer* commandBuffer, std::shared_ptr<Mesh> mesh);

#pragma endregion
};