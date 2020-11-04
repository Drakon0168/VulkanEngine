#pragma once
#include "pch.h"
#include "Mesh.h"

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

public:
#pragma region singleton
	static MeshManager* GetInstance();
#pragma endregion

#pragma region constructor
	MeshManager();
	void Init();
#pragma endregion

#pragma region accessors
	std::vector<std::shared_ptr<Mesh>> GetMeshes();
#pragma endregion
#pragma region meshLoading
	void LoadMeshes();
	void CreateMeshResources();
#pragma endregion

#pragma region drawing
	void DrawCube(glm::vec3 position);
	void AddCubeToRenderList(glm::mat4x4 model);
#pragma endregion

#pragma region cleanup
	void CleanupMeshes();
#pragma endregion
};

