#pragma once

#include "pch.h"
#include "Vertex.h"
#include "Transform.h"
#include "Material.h"
#include "Buffer.h"
#include "UniformBufferObject.h"

class Mesh
{
private:
	//Vertices
	std::vector<Vertex> vertices;
	uint32_t vertexBufferOffset;
	std::shared_ptr<Buffer> vertexBuffer;

	//Indices
	std::vector<uint16_t> indices;
	uint32_t indexBufferOffset;
	std::shared_ptr<Buffer> indexBuffer;

	//Instances
	std::vector<std::shared_ptr<Transform>> instances;
	uint32_t activeInstanceCount;
	std::shared_ptr<Buffer> instanceBuffer;

	//Material
	std::shared_ptr<Material> material;


	bool instanceBufferDirty = true;
public:

#pragma region Constructor

	Mesh(
		std::shared_ptr<Material> material = nullptr,
		std::vector<Vertex> vertices = {},
		std::vector<uint16_t> indices = {},
		std::shared_ptr<Buffer> vertexBuffer = nullptr, uint32_t vertexBufferOffset = 0, 
		std::shared_ptr<Buffer> indexBuffer = nullptr, uint32_t indexBufferOffset = 0,
		std::vector<std::shared_ptr<Transform>> instances = std::vector<std::shared_ptr<Transform>>(), std::shared_ptr<Buffer> instanceBuffer = nullptr);

#pragma endregion

#pragma region Buffer Management

	/// <summary>
	/// Creates and allocates mesh resources
	/// </summary>
	void Init();

	/// <summary>
	/// Creates and allocates the instance buffer that will be used by this mesh
	/// </summary>
	void CreateInstanceBuffer();

	/// <summary>
	/// Creates and allocates the vertex buffer that will be used by this mesh
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// Creates and allocates the index buffer that will be used by this mesh
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// Cleans up all mesh resources
	/// </summary>
	void Cleanup();

	/// <summary>
	/// Updates the mesh's instance buffer
	/// </summary>
	void UpdateInstanceBuffer();

	/// <summary>
	/// Updates the mesh's vertex buffer 
	/// </summary>
	void UpdateVertexBuffer();

	/// <summary>
	/// Updates the mesh's index buffer
	/// </summary>
	void UpdateIndexBuffer();

	
#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the list of vertices associated with this mesh
	/// </summary>
	/// <returns>List of vertices</returns>
	std::vector<Vertex> GetVertices();

	/// <summary>
	/// Sets the list of vertices associated with this mesh
	/// </summary>
	/// <param name="value">The list to set vertices to</param>
	void SetVertices(std::vector<Vertex> value);

	/// <summary>
	/// Returns a shared pointer to the vertex buffer with this mesh's data
	/// </summary>
	/// <returns>The vertex buffer with this mesh's data</returns>
	std::shared_ptr<Buffer> GetVertexBuffer();

	/// <summary>
	/// The offset in the vertex buffer for this mesh's data
	/// </summary>
	/// <returns>The vertex buffer offset</returns>
	uint32_t GetVertexBufferOffset();

	/// <summary>
	/// Sets the vertex buffer and vertex offset 
	/// </summary>
	/// <param name="value">The vertex buffer to set to</param>
	/// <param name="offset">The offset within the buffer that this mesh's data is stored at</param>
	void SetVertexBuffer(std::shared_ptr<Buffer> value, uint32_t offset = 0);

	/// <summary>
	/// Returns the list of indices associated with this mesh
	/// </summary>
	/// <returns>List of indices</returns>
	std::vector<uint16_t> GetIndices();

	/// <summary>
	/// Sets the list of indices associated with this mesh
	/// </summary>
	/// <param name="value">The list to set indices to</param>
	void SetIndices(std::vector<uint16_t> value);

	/// <summary>
	/// Returns a shared pointer to the index buffer with this mesh's data
	/// </summary>
	/// <returns>The index buffer with this mesh's data</returns>
	std::shared_ptr<Buffer> GetIndexBuffer();

	/// <summary>
	/// The offset in the index buffer for this mesh's data
	/// </summary>
	/// <returns>The index buffer offset</returns>
	uint32_t GetIndexBufferOffset();

	/// <summary>
	/// Sets the index buffer and index offset
	/// </summary>
	/// <param name="value">The index buffer to set to</param>
	/// <param name="offset">The offset within the buffer that this mesh's data is stored at</param>
	void SetIndexBuffer(std::shared_ptr<Buffer> value, uint32_t offset = 0);

	/// <summary>
	/// Returns the number of active instances of this mesh
	/// </summary>
	/// <returns>The number of active mesh instances</returns>
	uint32_t GetActiveInstanceCount();

	/// <summary>
	/// Returns a std::vector of all of the active instances of this mesh
	/// </summary>
	std::vector<std::shared_ptr<Transform>> GetActiveInstances();

	/// <summary>
	/// Returns the instance buffer used by this mesh
	/// </summary>
	/// <returns>The mesh's instance buffer</returns>
	std::shared_ptr<Buffer> GetInstanceBuffer();

	/// <summary>
	/// Sets the instance buffer to the specified value
	/// </summary>
	/// <param name="value">The value to set the instance buffer to</param>
	void SetInstanceBuffer(std::shared_ptr<Buffer> value);

	/// <summary>
	/// Returns the material that is being used by this mesh
	/// </summary>
	/// <returns>The material used by this mesh</returns>
	std::shared_ptr<Material> GetMaterial();

	/// <summary>
	/// Sets the material used by this mesh
	/// </summary>
	/// <param name="value">The material to use</param>
	void SetMaterial(std::shared_ptr<Material> value);

#pragma endregion

#pragma region Instances

	/// <summary>
	/// Adds the specified transform to the instance list
	/// </summary>
	/// <param name="value">The transform to add</param>
	/// <returns>The instance ID of the instance that was created</returns>
	int AddInstance(std::shared_ptr<Transform> value);

	/// <summary>
	/// Removes the specified instance from the instance list
	/// </summary>
	/// <param name="instanceId">The index of the instance to remove</param>
	void RemoveInstance(int instanceId);

#pragma endregion

#pragma region Mesh Generation

	/// <summary>
	/// Sets the vertices and indices to generate a plane
	/// </summary>
	void GeneratePlane();

	/// <summary>
	/// Sets the vertices and indices to generate a cube
	/// </summary>
	void GenerateCube();

	/// <summary>
	/// Sets the vertices and indices to generate a sphere
	/// </summary>
	void GenerateSphere(int resolution = 5);

	/// <summary>
	/// Sets the vertices and indicee to generate a line between two points
	/// </summary>
	void GenerateLine(glm::vec3 point1, glm::vec3 point2);

	/// <summary>
	/// Loads the model specified by the model path
	/// </summary>
	/// <param name="modelPath">The path to the model file from the project directory</param>
	void LoadModel(const std::string modelPath);

#pragma endregion
};