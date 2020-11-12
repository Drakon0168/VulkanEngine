#pragma once
#include "pch.h"
#include "Mesh.h"
#include "MeshData.h"
#include "GameObject.h"

/// <summary>
/// The purpose of this class is to provide easy ways to draw various meshes
/// without going through the process of making a GameObject shared pointer with
/// the correct mesh.  The goal of this is to be un-obtrusive to the original.
/// It will simply be an extra layer of abstraction (if needed)
/// </summary>
class MeshManager
{
private:
	inline static MeshManager* instance;

	// MeshManager should know about all the meshes somehow...
	// Solution: MeshManager will now handle loading all meshes, which EntityManager
	// will retrieve as needed
	std::vector<std::shared_ptr<Mesh>> meshes;

	// Instance buffers for each mesh's color (if needed:: ideally this will be for ALL shapes)
	std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> instanceBuffers;
	std::map<std::shared_ptr<Mesh>, bool> instanceBufferDirty;

	// TEMPORARY: Holds color data
	std::map<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<MeshData>>> meshData;
	std::vector<int> meshInstances;

public:
#pragma region singleton
	static MeshManager* GetInstance();
#pragma endregion

#pragma region constructor
	MeshManager();
	void Init();
	void Update();
#pragma endregion

#pragma region accessors
	std::vector<std::shared_ptr<Mesh>> GetMeshes();

	/// <summary>
	/// Returns the instance buffer map used by the mesh shapes
	/// </summary>
	/// <returns>The instance buffer map</returns>
	std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> GetInstanceBuffers();
#pragma endregion

#pragma region buffer handling
	/// <summary>
	/// Creates the instance buffer for the specified mesh
	/// </summary>
	/// <param name="mesh">The mesh to make an instance buffer for</param>
	void CreateInstanceBuffer(std::shared_ptr<Mesh> mesh);

	/// <summary>
	/// Update the instance buffer of the mesh
	/// </summary>
	/// <param name="mesh">The mesh to update the instance buffer</param>
	void UpdateInstanceBuffer(std::shared_ptr<Mesh> mesh);

	/// <summary>
	/// Removes meshData from the list of mesh Data
	/// </summary>
	/// <param name="mesh">The mesh that the Data applies to</param>
	/// <param name="index">The index of the Data to remove</param>
	void RemoveMeshData(std::shared_ptr<Mesh> mesh, int index);

	/// <summary>
	/// Adds MeshData to the the list for the specified mesh
	/// </summary>
	/// <param name="mesh">The mesh that the Data applies to</param>
	/// <param name="shape">The Data to add</param>
	void AddMeshData(std::shared_ptr<Mesh> mesh, std::shared_ptr<MeshData> data);
#pragma endregion

#pragma region meshLoading
	void LoadMeshes();
	void CreateMeshResources();
#pragma endregion

#pragma region gameObject creation
	std::shared_ptr<GameObject> MakeGameObject(MeshTypes type = MeshTypes::Cube);
#pragma endregion

#pragma region drawing
	void DrawCube(glm::vec3 position);
	void DrawWireCube(glm::vec3 position = glm::vec3(0), glm::vec3 color = glm::vec3(1));
	void AddCubeToRenderList(glm::mat4x4 model);
	void ClearRenderList();
#pragma endregion

#pragma region cleanup
	void CleanupMeshes();
#pragma endregion
};

