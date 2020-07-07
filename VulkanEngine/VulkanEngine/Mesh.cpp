#include "pch.h"
#include "Mesh.h"

#include "VulkanManager.h"

#define logicalDevice VulkanManager::GetInstance()->GetLogicalDevice()

#pragma region Memory Management

Mesh::Mesh(std::shared_ptr<Material> material, std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<Buffer> vertexBuffer, uint32_t vertexOffset, std::shared_ptr<Buffer> indexBuffer, uint32_t indexOffset, std::shared_ptr<Buffer> instanceBuffer, std::vector<std::shared_ptr<Transform>> instances)
{
	this->material = material;

	this->vertices = vertices;
	this->vertexOffset = vertexOffset;
	this->vertexBuffer = vertexBuffer;

	this->indices = indices;
	this->indexOffset = indexOffset;
	this->indexBuffer = indexBuffer;

	this->instanceBuffer;
	this->instances = instances;
}

void Mesh::Init()
{
	//Create the texture image
	//CreateTextureImage();

	//Create the texture's image view
	//CreateTextureImageView();

	CreateInstanceBuffer();

	CreateVertexBuffer();

	CreateIndexBuffer();
}

void Mesh::CreateInstanceBuffer()
{
	if (instances.size() < 1) {
		instances.push_back(std::make_shared<Transform>());
	}

	//Get Data as TransformData
	std::vector<TransformData> bufferData(instances.size());

	for (size_t i = 0; i < instances.size(); i++) {
		bufferData[i] = TransformData::LoadMat4(instances[i]->GetModelMatrix());
	}

	//Create staging buffer
	VkDeviceSize bufferSize = sizeof(TransformData) * bufferData.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Copy Buffer Data
	instanceBuffer = std::make_shared<Buffer>();
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *instanceBuffer);
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), instanceBuffer->GetBuffer(), bufferSize);

	//Cleanup
	stagingBuffer.Cleanup();
}

void Mesh::CreateVertexBuffer()
{
	//Create the staging buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Map vertex data to the buffer
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Create the vertex buffer
	vertexBuffer = std::make_shared<Buffer>();
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer);

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
}

void Mesh::CreateIndexBuffer()
{
	//Create the staging buffer
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Map index data to the buffer
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Create the index buffer
	indexBuffer = std::make_shared<Buffer>();
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer);

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
}

void Mesh::Cleanup()
{
	vertexBuffer->Cleanup();
	indexBuffer->Cleanup();
	instanceBuffer->Cleanup();
}

#pragma endregion

#pragma region Accessors

std::vector<Vertex> Mesh::GetVertices()
{
	return vertices;
}

void Mesh::SetVertices(std::vector<Vertex> value)
{
	vertices = value;
}

std::shared_ptr<Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

uint32_t Mesh::GetVertexBufferOffset()
{
	return vertexOffset;
}

void Mesh::SetVertexBuffer(std::shared_ptr<Buffer> value, uint32_t offset)
{
	vertexBuffer = value;
	vertexOffset = offset;
}

std::vector<uint16_t> Mesh::GetIndices()
{
	return indices;
}

void Mesh::SetIndices(std::vector<uint16_t> value)
{
	indices = value;
}

std::shared_ptr<Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

uint32_t Mesh::GetIndexBufferOffset()
{
	return indexOffset;
}

void Mesh::SetIndexBuffer(std::shared_ptr<Buffer> value, uint32_t offset)
{
	indexBuffer = value;
	indexOffset = offset;
}

std::shared_ptr<Buffer> Mesh::GetInstanceBuffer()
{
	return instanceBuffer;
}

void Mesh::SetInstanceBuffer(std::shared_ptr<Buffer> value)
{
	instanceBuffer = value;
}

std::vector<std::shared_ptr<Transform>> Mesh::GetInstances()
{
	return instances;
}

void Mesh::SetInstances(std::vector<std::shared_ptr<Transform>> value)
{
	instances = value;
}

std::vector<std::shared_ptr<Transform>> Mesh::GetActiveInstances()
{
	activeInstanceCount = 0;
	std::vector<std::shared_ptr<Transform>> activeInstances;

	for (int i = 0; i < instances.size(); i++) {
		if (instances[i] != nullptr) {
			activeInstances.push_back(instances[i]);
			activeInstanceCount++;
		}
	}

	return activeInstances;
}

uint32_t Mesh::GetActiveInstanceCount()
{
	return activeInstanceCount;
}

std::shared_ptr<Material> Mesh::GetMaterial()
{
	return material;
}

void Mesh::SetMaterial(std::shared_ptr<Material> value)
{
	material = value;
}

void Mesh::Draw(VkCommandBuffer* commandBuffer)
{
}

#pragma endregion

#pragma region Instances

void Mesh::AddInstance(std::shared_ptr<Transform> value)
{
	activeInstanceCount++;

	//Check for empty indexes that can be reused
	int emptyIndex = -1;
	for (int i = 0; i < instances.size(); i++) {
		if (instances[i] == nullptr) {
			emptyIndex = i;
			break;
		}
	}

	//Add to empty index
	//If no empty indices were found add to the end
	if (emptyIndex == -1) {
		instances.push_back(value);
	}

	//Otherwise add to empty index
	instances[emptyIndex] = value;
}

void Mesh::RemoveInstance(int instanceId)
{
	activeInstanceCount--;
	instances[instanceId] = nullptr;
}

#pragma endregion

#pragma region Buffer Management

void Mesh::UpdateInstanceBuffer()
{
	VkDeviceSize bufferSize = sizeof(TransformData) * instances.size();
	std::vector<TransformData> bufferData(instances.size());

	for (size_t i = 0; i < instances.size(); i++) {
		bufferData[i] = TransformData::LoadMat4(instances[i]->GetModelMatrix());
	}

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, instanceBuffer->GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(logicalDevice, instanceBuffer->GetBufferMemory());
}

void Mesh::UpdateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Copy Buffer Data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

	//Cleanup
	stagingBuffer.Cleanup();
}

void Mesh::UpdateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Copy Buffer Data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

	//Cleanup
	stagingBuffer.Cleanup();
}

#pragma endregion

#pragma region Mesh Generation

void Mesh::GeneratePlane()
{
	//Set vertices
	vertices.resize(4);

	vertices = {
		{{-0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0, 1, 2,
		2, 3, 0
	};

	//UpdateVertexBuffer();
	//UpdateIndexBuffer();
}

void Mesh::GenerateCube()
{
	//Set vertices
	vertices.resize(4);

	vertices = {
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0,5,1,
		0,4,5,
		2,3,7,
		2,7,6,
		1,2,0,
		1,3,2,
		4,0,2,
		4,2,6,
		5,4,6,
		5,6,7,
		5,3,1,
		5,7,3
	};

	//UpdateVertexBuffer();
	//UpdateIndexBuffer();
}

void Mesh::GenerateSphere(uint32_t resolution)
{
	//Set minimum resolution of 3
	if (resolution < 3) {
		resolution = 3;
	}

	vertices.clear();
	indices.clear();

	//Set Vertices
	float angleOffset = glm::radians(360.0f) / resolution;
	float heightAngleOffset = glm::radians(180.0f) / (resolution - 1);
	float radius = 0.5f;

	//Add cap
	vertices.push_back(Vertex(glm::vec3(0.0f, radius, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)));

	for (uint32_t row = 1; row < resolution - 1; row++) {
		float height = cosf(heightAngleOffset * row) * radius;
		float width = sinf(heightAngleOffset * row) * radius;

		for (uint16_t col = 0; col < resolution; col++) {
			vertices.push_back(Vertex(
				glm::vec3(cosf(angleOffset * col) * width, height, sinf(angleOffset * col) * width),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec2((float)col / resolution, (float)height / resolution)));
		}
	}

	//Add bottom
	vertices.push_back(Vertex(glm::vec3(0.0f, -radius, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)));

	//Set Indices
	//Add Cap
	for (uint16_t i = 1; i <= resolution; i++) {
		if (i == 1) {
			indices.push_back(0);
			indices.push_back(resolution);
			indices.push_back(i);
		}
		else {
			indices.push_back(0);
			indices.push_back(i - 1);
			indices.push_back(i);
		}
	}

	int rowCount = (vertices.size() - 2) / resolution;
	int rowStart = 1;
	int rowEnd = resolution;

	if (rowCount > 1) {
		for (int i = 0; i < rowCount - 1; i++) {
			rowStart = 1 + (i * resolution);
			rowEnd = rowStart + resolution - 1;

			for (int j = rowStart; j <= rowEnd; j++) {
				if (j == rowStart) {
					indices.push_back(j);
					indices.push_back(rowEnd);
					indices.push_back(rowEnd + resolution);
					indices.push_back(j);
					indices.push_back(rowEnd + resolution);
					indices.push_back(j + resolution);
				}
				else {
					indices.push_back(j);
					indices.push_back(j - 1);
					indices.push_back((j - 1) + resolution);
					indices.push_back(j);
					indices.push_back((j - 1) + resolution);
					indices.push_back(j + resolution);
				}
			}
		}
	}

	//Add Bottom
	rowStart = (rowCount - 1) * resolution;
	rowEnd = rowStart + resolution - 1;

	for (uint16_t i = 0; i < resolution; i++) {
		if (i == 0) {
			indices.push_back(vertices.size() - 1);
			indices.push_back(rowStart);
			indices.push_back(rowEnd);
		}
		else {
			indices.push_back(vertices.size() - 1);
			indices.push_back(rowStart + i);
			indices.push_back(rowStart + i - 1);
		}
	}

	//UpdateVertexBuffer();
	//UpdateIndexBuffer();
}

#pragma endregion