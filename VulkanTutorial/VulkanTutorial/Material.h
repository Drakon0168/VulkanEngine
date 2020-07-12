#pragma once
class Material
{
private:
	char* vertexShaderPath;
	char* fragmentShaderPath;

	std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorPool descriptorPool;

public:

#pragma region Constructor

	Material(char* vertexShaderPath = (char*)"shaders/vert.spv", char* fragmentShaderPath = (char*)"shaders/frag.spv");

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the path to the vertex shader file used by this material
	/// </summary>
	/// <returns>The file path to the vertex shader</returns>
	char* GetVertexShaderPath();

	/// <summary>
	/// Sets the vertex shader used by this material
	/// </summary>
	/// <param name="value">The file path to the shader file</param>
	void SetVertexShaderPath(char* value);

	/// <summary>
	/// Returns the path to the fragment shader file used by this material
	/// </summary>
	/// <returns>The file path to the fragment shader</returns>
	char* GetFragmentShaderPath();

	/// <summary>
	/// Sets the fragment shader used by this material
	/// </summary>
	/// <param name="value">The file path to the shader file</param>
	void SetFragmentShaderPath(char* value);

	/// <summary>
	/// Returns the list of descriptor sets used by this material
	/// </summary>
	/// <returns>The descriptor sets used by the material</returns>
	std::vector<VkDescriptorSet> GetDescriptorSets();

	/// <summary>
	/// Returns the descriptor pool that is used by this material
	/// </summary>
	/// <returns>The material's descriptor pool</returns>
	VkDescriptorPool GetDescriptorPool();

#pragma endregion

#pragma region Resource Management

	/// <summary>
	/// Initializes material resources
	/// </summary>
	void Init();

	/// <summary>
	/// Creates and allocates the descriptor pool used by this material
	/// </summary>
	void CreateDescriptorPool();

	/// <summary>
	/// Creates and allocates the descriptor sets used by this material
	/// </summary>
	void CreateDescriptorSets();

	/// <summary>
	/// Cleans up material resources
	/// </summary>
	void Cleanup();

#pragma endregion
};