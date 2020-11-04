#include "pch.h"
#include "MeshManager.h"
#include "EntityManager.h"

MeshManager* MeshManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new MeshManager();
	}
	return instance;
}

MeshManager::MeshManager()
{
	Init();
}

void MeshManager::Init()
{
	LoadMeshes();
    std::vector<std::shared_ptr<Material>> materials = EntityManager::GetInstance()->GetMaterials();
    meshes.resize(MeshTypes::MeshTypeCount);

    meshes[MeshTypes::Plane] = std::make_shared<Mesh>(materials[0]);
    meshes[MeshTypes::Plane]->GeneratePlane();

    meshes[MeshTypes::Cube] = std::make_shared<Mesh>(materials[1]);
    meshes[MeshTypes::Cube]->GenerateCube();

    meshes[MeshTypes::Sphere] = std::make_shared<Mesh>(materials[0]);
    meshes[MeshTypes::Sphere]->GenerateSphere(50);

    meshes[MeshTypes::Model] = std::make_shared<Mesh>(materials[1]);
    meshes[MeshTypes::Model]->LoadModel("models/room.obj");

    meshes[MeshTypes::Skybox] = std::make_shared<Mesh>(materials[2]);
    meshes[MeshTypes::Skybox]->GenerateCube();

    meshes[MeshTypes::WireCube] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::WireCube]->GenerateCube();

    meshes[MeshTypes::WireSphere] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::WireSphere]->GenerateSphere(10);

    meshes[MeshTypes::Line] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::Line]->GenerateLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

std::vector<std::shared_ptr<Mesh>> MeshManager::GetMeshes()
{
	return meshes;
}

void MeshManager::LoadMeshes()
{
	// Mesh loading code
}

void MeshManager::CreateMeshResources()
{
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i]->Init();
	}
}

void MeshManager::DrawCube(glm::vec3 position)
{
	
}

void MeshManager::AddCubeToRenderList(glm::mat4x4 model)
{
}

void MeshManager::CleanupMeshes()
{
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->Cleanup();
    }
}
