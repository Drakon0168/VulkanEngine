#include "pch.h"
#include "MeshManager.h"
#include "EntityManager.h"
# include "VulkanManager.h"

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
}

void MeshManager::Update()
{
	for (std::pair<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<MeshData>>> pair : meshData) {
		/*if (instanceBuffers[pair.first] != nullptr) {
			UpdateInstanceBuffer(pair.first);
		}*/
		for (int i = 0; i < pair.second.size(); i++)
		{
			pair.second[i]->transform->SetPosition(pair.second[i]->parent->GetPosition());
		}
	}
	prevDrawingDebug = drawingDebug;
}

std::vector<std::shared_ptr<Mesh>> MeshManager::GetMeshes()
{
	return meshes;
}

std::map<std::shared_ptr<Mesh>, std::shared_ptr<Buffer>> MeshManager::GetInstanceBuffers()
{
    return instanceBuffers;
}

void MeshManager::ToggleDebug()
{
	drawingDebug = !drawingDebug;
	if (drawingDebug == false)
	{
		RemoveDebugShapes();
	}
}

bool MeshManager::GetDrawHandles()
{
	return drawingDebug && !prevDrawingDebug;
}

#pragma region buffer handling
void MeshManager::CreateInstanceBuffer(std::shared_ptr<Mesh> mesh)
{
    //Create buffer
    VkDeviceSize bufferSize = sizeof(glm::vec3);
    instanceBuffers[mesh] = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
    Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffers[mesh]);

    //Data will be added to the buffer in UpdateInstanceBuffer method once we have data to add
}

void MeshManager::UpdateInstanceBuffer(std::shared_ptr<Mesh> mesh)
{
	//If a new object has been spawned or deleted the instance buffer must be re-created to the correct size
	if (instanceBufferDirty[mesh]) {
		vkQueueWaitIdle(VulkanManager::GetInstance()->GetGraphicsQueue());
		instanceBuffers[mesh]->Cleanup();
		instanceBuffers[mesh] = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
	}

	//Get Color data
	std::vector<glm::vec3> bufferData;

	for (size_t i = 0; i < meshData[mesh].size(); i++) {
		if (meshData[mesh][i] != nullptr) {
			bufferData.push_back(meshData[mesh][i]->color);
		}
	}

	//Ensure that buffer size is not 0
	VkDeviceSize bufferSize;
	if (bufferData.size() > 0) {
		bufferSize = sizeof(glm::vec3) * bufferData.size();
	}
	else {
		bufferData.push_back(glm::vec3(0, 0, 0));
		bufferSize = sizeof(glm::vec3);
	}

	//Create the buffer if necessary
	if (instanceBufferDirty[mesh]) {
		Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffers[mesh]);
	}
	//Copy Data
	void* data;
	vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), instanceBuffers[mesh]->GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), instanceBuffers[mesh]->GetBufferMemory());

	instanceBufferDirty[mesh] = false;
}

void MeshManager::RemoveMeshData(std::shared_ptr<Mesh> mesh, int index)
{
	instanceBufferDirty[mesh] = true;
	mesh->RemoveInstance(meshData[mesh][index]->meshID);
	meshData[mesh][index] = nullptr;
}

void MeshManager::AddMeshData(std::shared_ptr<Mesh> mesh, std::shared_ptr<MeshData> data)
{
	instanceBufferDirty[mesh] = true;

	for (size_t i = 0; i < meshData[mesh].size(); i++) {
		if (meshData[mesh][i] == nullptr) {
			meshData[mesh][i] = data;
			return;
		}
	}

	meshData[mesh].push_back(data);
}
#pragma endregion


void MeshManager::LoadMeshes()
{
	// Mesh loading code

    // Get materials from Entity Manager
    std::vector<std::shared_ptr<Material>> materials = EntityManager::GetInstance()->GetMaterials();
    meshes.resize(MeshTypes::MeshTypeCount);

    // Each mesh must be created with a particular material.  Each material uses a shader
    // and is created in the EntityManager.  Go there if you wish to swap out shaders (binding 
    // descriptions must match)
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

void MeshManager::CreateMeshResources()
{
	for (size_t i = 0; i < meshes.size(); i++) {
		meshes[i]->Init();
	}
}

std::shared_ptr<GameObject> MeshManager::MakeGameObject(MeshTypes type)
{
    return std::make_shared<GameObject>(meshes[type]);
}

void MeshManager::DrawCube(glm::vec3 position)
{
    meshes[MeshTypes::Cube]->AddInstance(std::make_shared<Transform>(position), glm::vec3(0.0f, 0.0f, 0.0f));
}

void MeshManager::DrawWireCube(glm::vec3 position, glm::vec3 color)
{
	// std::shared_ptr<Mesh> mesh = meshes[MeshTypes::WireCube];
	// 
	// std::shared_ptr<MeshData> data = std::make_shared<MeshData>();
	// data->transform = std::make_shared<Transform>(position, glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
	// data->color = color;
	// data->meshID = mesh->AddInstance(data->transform);
	// 
	// AddMeshData(mesh, data);
	// 
	// if (instanceBuffers[mesh] == nullptr) {
	// 	CreateInstanceBuffer(mesh);
	// }
    meshes[MeshTypes::WireCube]->AddInstance(std::make_shared<Transform>(position), color);
}

void MeshManager::AddCubeToRenderList(glm::mat4x4 model)
{
}

void MeshManager::ClearRenderList()
{
	for (std::pair<std::shared_ptr<Mesh>, std::vector<std::shared_ptr<MeshData>>> pair : meshData) {
		for (int i = 0; i < pair.second.size(); i++) {
			if (pair.second[i] == nullptr) {
				continue;
			}

			RemoveMeshData(pair.first, i);
		}

		// if (instanceBuffers[pair.first] != nullptr) {
		// 	UpdateInstanceBuffer(pair.first);
		// }
	}
}

void MeshManager::DrawDebugWireSphere(glm::vec3 position, glm::vec3 color, float radius)
{
	std::shared_ptr<Mesh> mesh = meshes[MeshTypes::WireSphere];
	debugMeshInstances[mesh].push_back(
		mesh->AddInstance(
		std::make_shared<Transform>(
			position, glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(radius / 0.5f, radius / 0.5f, radius / 0.5f)), 
			color));


}

void MeshManager::DrawDebugWireCube(glm::vec3 position, glm::vec3 color, glm::vec3 size)
{
	std::shared_ptr<Mesh> mesh = meshes[MeshTypes::WireCube];

	debugMeshInstances[mesh].push_back(
		mesh->AddInstance(
			std::make_shared<Transform>(position, glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), size),
			color));

}

void MeshManager::DrawDebugLine(Transform* center, glm::vec3 position1, glm::vec3 position2, glm::vec3 color)
{
	std::shared_ptr<Mesh> mesh = meshes[MeshTypes::Line];

	glm::vec3 offset = center->GetPosition();
	position1 += offset;
	position2 += offset;
	float length = glm::distance(position1, position2);
	glm::vec3 direction = (position1 - position2) / length;
	glm::quat orientation;
	if (direction == glm::vec3(0.0f, 1.0f, 0.0f)) {
		orientation = glm::angleAxis((3.14f / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (direction == glm::vec3(0.0f, -1.0f, 0.0f)) {
		orientation = glm::angleAxis((-3.14f / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else {
		orientation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	std::shared_ptr<MeshData> temp = std::make_shared<MeshData>();
	temp->transform = std::make_shared<Transform>(position1, orientation, glm::vec3(1.0f, 1.0f, 1.0f) * length);
	temp->parent = center;
	temp->meshID = mesh->AddInstance(temp->transform,color);
	debugMeshInstances[mesh].push_back(temp->meshID);
	meshData[mesh].push_back(temp);

}



void MeshManager::RemoveDebugShapes()
{
	for (std::pair<std::shared_ptr<Mesh>, std::vector<int>> pair : debugMeshInstances)
	{
		for (int i = 0; i < pair.second.size(); i++)
		{
			pair.first->RemoveInstance(pair.second[i]);
		}
		pair.second.clear();
	}
	debugMeshInstances.clear();
}

void MeshManager::CleanupMeshes()
{
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->Cleanup();
		if (instanceBuffers[meshes[i]] != nullptr) instanceBuffers[meshes[i]]->Cleanup();
    }
}
