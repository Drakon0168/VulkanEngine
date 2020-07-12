#pragma once
class Material
{
private:
	char* vertexShaderPath;
	char* fragmentShaderPath;



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

#pragma endregion
};