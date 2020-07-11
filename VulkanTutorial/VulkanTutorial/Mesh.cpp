#include "pch.h"
#include "Mesh.h"

#include "TriangleApp.h"
#include "TransformData.h"

#define logicalDevice TriangleApp::logicalDevice

#pragma region Constructor

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<Buffer> vertexBuffer, uint32_t vertexBufferOffset, std::shared_ptr<Buffer> indexBuffer, uint32_t indexBufferOffset, std::vector<std::shared_ptr<Transform>> instances, std::shared_ptr<Buffer> instanceBuffer)
{
	this->vertices = vertices;
	this->indices = indices;
	this->vertexBuffer = vertexBuffer;
	this->vertexBufferOffset = vertexBufferOffset;
	this->indexBuffer = indexBuffer;
	this->indexBufferOffset = indexBufferOffset;
	this->instances = instances;
	this->instanceBuffer = instanceBuffer;

	activeInstanceCount = static_cast<uint32_t>(instances.size());
}

#pragma endregion

#pragma region Buffer Management

void Mesh::CreateInstanceBuffer()
{
	//Get Data as TransformData
	std::vector<std::shared_ptr<Transform>> activeInstances = GetActiveInstances();
	std::vector<TransformData> bufferData(activeInstanceCount);

	for (size_t i = 0; i < activeInstanceCount; i++) {
		bufferData[i] = TransformData::LoadMat4(activeInstances[i]->GetModelMatrix());
	}

	//Create buffer
	VkDeviceSize bufferSize = sizeof(TransformData) * bufferData.size();
	instanceBuffer = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffer);

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, instanceBuffer->GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(logicalDevice, instanceBuffer->GetBufferMemory());
}

void Mesh::UpdateInstanceBuffer()
{
	//Get Data as TransformData
	std::vector<std::shared_ptr<Transform>> activeInstances = GetActiveInstances();
	std::vector<TransformData> bufferData(activeInstanceCount);

	for (size_t i = 0; i < activeInstanceCount; i++) {
		bufferData[i] = TransformData::LoadMat4(activeInstances[i]->GetModelMatrix());
	}

	VkDeviceSize bufferSize = sizeof(TransformData) * bufferData.size();

	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, instanceBuffer->GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(logicalDevice, instanceBuffer->GetBufferMemory());
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
	return vertexBufferOffset;
}

void Mesh::SetVertexBuffer(std::shared_ptr<Buffer> value, uint32_t offset)
{
	vertexBuffer = value;
	vertexBufferOffset = offset;
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
	return indexBufferOffset;
}

void Mesh::SetIndexBuffer(std::shared_ptr<Buffer> value, uint32_t offset)
{
	indexBuffer = value;
	indexBufferOffset = offset;
}

uint32_t Mesh::GetActiveInstanceCount()
{
	return activeInstanceCount;
}

std::vector<std::shared_ptr<Transform>> Mesh::GetActiveInstances()
{
	std::vector<std::shared_ptr<Transform>> activeInstances;
	activeInstanceCount = 0;

	for (size_t i = 0; i < instances.size(); i++) {
		if (instances[i] != nullptr) {
			activeInstances.push_back(instances[i]);
			activeInstanceCount++;
		}
	}

	return activeInstances;
}

std::shared_ptr<Buffer> Mesh::GetInstanceBuffer()
{
	return instanceBuffer;
}

void Mesh::SetInstanceBuffer(std::shared_ptr<Buffer> value)
{
	instanceBuffer = value;
}

#pragma endregion

#pragma region Mesh Generation

void Mesh::AddInstance(std::shared_ptr<Transform> value)
{
	activeInstanceCount++;
	size_t freeIndex = -1;

	for (size_t i = 0; i < instances.size(); i++) {
		if (instances[i] == nullptr) {
			freeIndex = i;
			break;
		}
	}

	if (freeIndex == -1) {
		instances.push_back(value);
		return;
	}

	instances[freeIndex] = value;
}

void Mesh::RemoveInstance(int instanceId)
{
	if (instanceId < 0 || instanceId >= instances.size()) {
		throw std::runtime_error("Failed to remove instance Id out of bounds!");
	}

	activeInstanceCount--;
	instances[instanceId] == nullptr;
}

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

	UpdateBuffers();
}

void Mesh::GenerateCube() {
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

	UpdateBuffers();
}

void Mesh::GenerateSphere(int resolution)
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
	vertices.push_back(Vertex(glm::vec3(0.0f, -radius, 0.0f), glm::vec3(1.0f, 1.0f,1.0f), glm::vec2(0.0f, 0.0f)));

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

	UpdateBuffers();
}

#pragma endregion

#pragma region Buffer Management

void Mesh::UpdateBuffers()
{
	//TODO: Update buffers with accurate mesh data
}

#pragma endregion