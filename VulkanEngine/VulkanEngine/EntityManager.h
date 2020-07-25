#pragma once
#include "pch.h"

#include "Material.h"
#include "Mesh.h"
#include "Buffer.h"

class EntityManager
{
private:
	static EntityManager* instance;

	std::map<std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>> entities;

	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Mesh>> meshes;

public:
#pragma region Singleton

	/// <summary>
	/// Returns the singleton instance of the Entity Manager
	/// </summary>
	/// <returns>The singleton instance</returns>
	static EntityManager* GetInstance();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns a list of all of the materials in use
	/// </summary>
	/// <returns>std::vector<std::shared_ptr<Material>> of the materials that are in use</returns>
	std::vector<std::shared_ptr<Material>> GetMaterials();

	/// <summary>
	/// Returns a list of all of the meshes in use
	/// </summary>
	/// <returns>std::vector<std::shared_ptr<Mesh>> of the meshes that are in use</returns>
	std::vector<std::shared_ptr<Mesh>> GetMeshes();

#pragma endregion

#pragma region Initialization

	/// <summary>
	/// Loads all meshes and materials and sets up the entity map
	/// </summary>
	void Init();

	/// <summary>
	/// Loads all meshes and attaches them to the appropriate materials
	/// </summary>
	void LoadMeshes();

	/// <summary>
	/// Loads all materials
	/// </summary>
	void LoadMaterials();

#pragma endregion

#pragma region Game Loop

	/// <summary>
	/// Updates instance lists on all meshes
	/// </summary>
	void Update();

	/// <summary>
	/// Re-records the command buffer to draw all meshes
	/// </summary>
	/// <param name="commandBuffer">The command buffer that will be recorded</param>
	void Draw(uint32_t imageIndex, VkCommandBuffer* commandBuffer);

#pragma endregion

#pragma region Memory Management

	/// <summary>
	/// Creates and allocates the resources on all materials
	/// </summary>
	void CreateMaterialResources();

	/// <summary>
	/// Creates and allocates the resources on all meshes
	/// </summary>
	void CreateMeshResources();

	/// <summary>
	/// Cleans up the resources on all entities
	/// </summary>
	void Cleanup();

	/// <summary>
	/// Cleans up all material resources
	/// </summary>
	void CleanupMaterials();

	/// <summary>
	/// Cleans up all mesh resources
	/// </summary>
	void CleanupMeshes();

#pragma endregion
};