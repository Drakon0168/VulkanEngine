#pragma once
#include "pch.h"

#include "Transform.h"
#include "Buffer.h"
#include "Material.h"

class Mesh
{
private:
	std::vector<Vertex> vertices;
	uint32_t vertexOffset;
	std::shared_ptr<Buffer> vertexBuffer;

	std::vector<uint16_t> indices;
	uint32_t indexOffset;
	std::shared_ptr<Buffer> indexBuffer;

	std::shared_ptr<Buffer> instanceBuffer;
	uint32_t activeInstanceCount;
	std::vector<std::shared_ptr<Transform>> instances;

	std::shared_ptr<Material> material;

public:

#pragma region Memory Management

	Mesh(
		std::shared_ptr<Material> material = nullptr,
		std::vector<Vertex> vertices = std::vector<Vertex>(),
		std::vector<uint16_t> indices = std::vector<uint16_t>(),
		std::shared_ptr<Buffer> vertexBuffer = nullptr, uint32_t vertexOffset = 0,
		std::shared_ptr<Buffer> indexBuffer = nullptr, uint32_t indexOffset = 0,
		std::shared_ptr<Buffer> instanceBuffer = nullptr,
		std::vector<std::shared_ptr<Transform>> instances = std::vector<std::shared_ptr<Transform>>());

	/// <summary>
	/// Creates and allocates mesh resources
	/// </summary>
	void Init();

	/// <summary>
	/// Creates and allocates the instance buffer
	/// </summary>
	void CreateInstanceBuffer();

	/// <summary>
	/// Creates and allocates the vertex buffer
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// Creates and allocates the index buffer
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// Cleans up the mesh's resources
	/// </summary>
	void Cleanup();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the list of this mesh's vertices
	/// </summary>
	/// <returns>The mesh's vertices</returns>
	std::vector<Vertex> GetVertices();

	/// <summary>
	/// Sets the vertex list to the specified value
	/// </summary>
	/// <param name="value">The value to set the vertex list to</param>
	void SetVertices(std::vector<Vertex> value);

	/// <summary>
	/// Returns the vertex buffer used by this mesh
	/// </summary>
	/// <returns>A shared pointer to the mesh's vertex buffer</returns>
	std::shared_ptr<Buffer> GetVertexBuffer();

	/// <summary>
	/// Gets the offset into the vertex buffer (for when multiple mesh's share a vertex buffer)
	/// </summary>
	/// <returns>The offset into the vertex buffer</returns>
	uint32_t GetVertexBufferOffset();

	/// <summary>
	/// Sets the vertex buffer and offset into it
	/// </summary>
	/// <param name="value">The buffer to set to</param>
	/// <param name="offset">The offset into the buffer where this mesh's data starts</param>
	void SetVertexBuffer(std::shared_ptr<Buffer> value, uint32_t offset);

	/// <summary>
	/// Returns the list of indices
	/// </summary>
	/// <returns>The mesh's indices</returns>
	std::vector<uint16_t> GetIndices();

	/// <summary>
	/// Sets the index list of this mesh
	/// </summary>
	/// <param name="value">The index list to set to</param>
	void SetIndices(std::vector<uint16_t> value);

	/// <summary>
	/// Returns the index buffer used by the mesh
	/// </summary>
	/// <returns>The mesh's index buffer</returns>
	std::shared_ptr<Buffer> GetIndexBuffer();

	/// <summary>
	/// Returns the offset into the index buffer of this mesh's data
	/// </summary>
	/// <returns>The offset into the index buffer</returns>
	uint32_t GetIndexBufferOffset();

	/// <summary>
	/// Sets the index buffer and the offset into it
	/// </summary>
	/// <param name="value">The value to set the index buffer to</param>
	/// <param name="offset">The offset into the buffer of this mesh's data</param>
	void SetIndexBuffer(std::shared_ptr<Buffer> value, uint32_t offset);

	/// <summary>
	/// Returns the mesh's instance buffer
	/// </summary>
	/// <returns>The mesh's instance buffer</returns>
	std::shared_ptr<Buffer> GetInstanceBuffer();

	/// <summary>
	/// Sets the instance buffer of the mesh
	/// </summary>
	/// <param name="value">The instance buffer to set to</param>
	void SetInstanceBuffer(std::shared_ptr<Buffer> value);

	/// <summary>
	/// Returns a list of the transforms of each instance of this mesh
	/// </summary>
	/// <returns>The list of instance transforms</returns>
	std::vector<std::shared_ptr<Transform>> GetInstances();

	/// <summary>
	/// Sets the instance list of the mesh
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetInstances(std::vector<std::shared_ptr<Transform>> value);

	/// <summary>
	/// Returns a list of the active instances of the mesh
	/// </summary>
	/// <returns>The active instances of the mesh</returns>
	std::vector<std::shared_ptr<Transform>> GetActiveInstances();

	/// <summary>
	/// Returns the number of active instances
	/// </summary>
	/// <returns>The number of active instances</returns>
	uint32_t GetActiveInstanceCount();

	/// <summary>
	/// Returns the material used by the mesh
	/// </summary>
	/// <returns>The mesh's material</returns>
	std::shared_ptr<Material> GetMaterial();

	/// <summary>
	/// Sets the material used by the mesh
	/// </summary>
	/// <param name="value">The value to set to</param>
	void SetMaterial(std::shared_ptr<Material> value);

#pragma endregion

#pragma region Draw

	void Draw(VkCommandBuffer* commandBuffer);

#pragma endregion

#pragma region Instances

	/// <summary>
	/// Adds a transform to the mesh's list of instances
	/// </summary>
	/// <param name="value">The transform to add to the list</param>
	void AddInstance(std::shared_ptr<Transform> value);

	/// <summary>
	/// Removes a transform from the list of instances
	/// </summary>
	/// <param name="instanceId">The index of the instance to remove from the list</param>
	void RemoveInstance(int instanceId);

#pragma endregion

#pragma region Buffer Management

	/// <summary>
	/// Updates the instance buffer with all of the active instances
	/// </summary>
	void UpdateInstanceBuffer();

	/// <summary>
	/// Updates the vertex buffer to match the vertex list
	/// </summary>
	void UpdateVertexBuffer();

	/// <summary>
	/// Updates the index buffer to match the index list
	/// </summary>
	void UpdateIndexBuffer();

#pragma endregion

#pragma region Mesh Generation

	/// <summary>
	/// Generates a plane and updates the buffers
	/// </summary>
	void GeneratePlane();

	/// <summary>
	/// Generates a cube and updates the buffers
	/// </summary>
	void GenerateCube();

	/// <summary>
	/// Generates a sphere and updates the buffers
	/// </summary>
	void GenerateSphere(uint32_t resolution);

#pragma endregion
};