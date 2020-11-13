#include "pch.h"
#include "Mesh.h"

#include "VulkanManager.h"
#include "TransformData.h"
#include "Image.h"
//Tiny OBJ Loader
#define TINYOBJLOADER_IMPLEMENTATION 
#include <TinyObjLoader/tiny_obj_loader.h>
#include "TextureImages.h"

#define logicalDevice VulkanManager::GetInstance()->GetLogicalDevice()

#pragma region Constructor
// WELCOME TO ATLAS!! <3 <3 
Mesh::Mesh(std::shared_ptr<Material> material, std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<Buffer> vertexBuffer, uint32_t vertexBufferOffset, std::shared_ptr<Buffer> indexBuffer, uint32_t indexBufferOffset, std::vector<std::shared_ptr<Transform>> instances, std::shared_ptr<Buffer> instanceBuffer)
{
	this->material = material;
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

void Mesh::Init()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateInstanceBuffer();
}

void Mesh::CreateInstanceBuffer()
{
	//Create buffer
	VkDeviceSize bufferSize = sizeof(TransformData);
	instanceBuffer = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffer);

	//Data will be added to the buffer in UpdateInstanceBuffer method once we have data to add
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

void Mesh::UpdateInstanceBuffer()
{
	//If a new object has been spawned or deleted the instance buffer must be re-created to the correct size
	if (instanceBufferDirty) {
		vkQueueWaitIdle(VulkanManager::GetInstance()->GetGraphicsQueue());
		instanceBuffer->Cleanup();
		instanceBuffer = std::make_shared<Buffer>(VkBuffer(), VkDeviceMemory());
	}

	//Get Data as TransformData
	std::vector<std::shared_ptr<Transform>> activeInstances = GetActiveInstances();
	std::vector<TransformData> bufferData(activeInstanceCount);

	for (size_t i = 0; i < activeInstanceCount; i++) {
		bufferData[i] = TransformData::LoadMat4(activeInstances[i]->GetModelMatrix());
	}

	//Ensure that buffer size is not 0
	VkDeviceSize bufferSize;
	if (bufferData.size() > 0) {
		bufferSize = sizeof(TransformData) * bufferData.size();
	}
	else {
		bufferSize = sizeof(TransformData);
		// return;
	}

	//Create the buffer if necessary
	if (instanceBufferDirty) {
		Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *instanceBuffer);
	}
	//Copy Data
	void* data;
	vkMapMemory(logicalDevice, instanceBuffer->GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), bufferSize);
	vkUnmapMemory(logicalDevice, instanceBuffer->GetBufferMemory());

	instanceBufferDirty = false;
}

void Mesh::UpdateVertexBuffer()
{
	//Create the staging buffer
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	Buffer stagingBuffer;

	Buffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	//Map index data to the buffer
	void* data;
	vkMapMemory(logicalDevice, stagingBuffer.GetBufferMemory(), 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), bufferSize);
	vkUnmapMemory(logicalDevice, stagingBuffer.GetBufferMemory());

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
}

void Mesh::UpdateIndexBuffer()
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

	//Copy buffer data
	Buffer::CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);

	//Cleanup staging buffer
	stagingBuffer.Cleanup();
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

std::shared_ptr<Material> Mesh::GetMaterial()
{
	return material;
}

void Mesh::SetMaterial(std::shared_ptr<Material> value)
{
	material = value;
}

#pragma endregion

#pragma region Instances

int Mesh::AddInstance(std::shared_ptr<Transform> value)
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
		return (instances.size() - 1);
	}
	instances[freeIndex] = value;
	instanceBufferDirty = true;
	return freeIndex;
}

void Mesh::RemoveInstance(int instanceId)
{
	if (instanceId < 0 || instanceId >= instances.size()) {
		throw std::runtime_error("Failed to remove instance, ID out of bounds!");
	}

	if (instances[instanceId] == nullptr) {
		throw std::runtime_error("Failed to remove instance, ID was empty!");
	}

	activeInstanceCount--;
	instances[instanceId] = nullptr;
	// We technically don't need to set the instanceColor at this index to anything bc
	// once a new instance is added to instances, the instanceColor will be overridden.
	instanceBufferDirty = true;
}

#pragma endregion

#pragma region Mesh Generation

void Mesh::GeneratePlane()
{
	//Set vertices
	vertices.resize(4);

	vertices = {
		{{-0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0, 2, 1,
		2, 0, 3
	};

	if (vertexBuffer != nullptr && indexBuffer != nullptr) {
		UpdateVertexBuffer();
		UpdateIndexBuffer();
	}
}

void Mesh::GenerateCube() {
	//Set vertices
	vertices.resize(4);

	/*vertices = {
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f}     , 1.0f },
		{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {1.0f, 1.0f}    , 0.0f},
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {1.0f, 0.0f}   , 1.0f },
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f}  , 0.0f},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {0.0f, 1.0f}   , 1.0f },
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f}  , 0.0f},
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} , 1.0f },
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f},
	};*/

	vertices = {
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f}  , 1.0f },
		{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {1.0f, 1.0f}  , 0.0f },
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {1.0f, 0.0f}  , 1.0f },
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f}  , 0.0f },
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},   {0.0f, 1.0f}  , 1.0f },
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f}  , 0.0f },
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f}  , 1.0f },
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}  , 0.0f },
	};

	//Set indices
	indices.resize(6);

	indices = {
		0,1,5,
		0,5,4,
		2,7,3,
		2,6,7,
		1,0,2,
		1,2,3,
		4,2,0,
		4,6,2,
		5,6,4,
		5,7,6,
		5,1,3,
		5,3,7
	};

	if (vertexBuffer != nullptr && indexBuffer != nullptr) {
		UpdateVertexBuffer();
		UpdateIndexBuffer();
	}
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
	vertices.push_back(Vertex(glm::vec3(0.0f, radius, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));

	for (uint32_t row = 1; row < resolution - 1; row++) {
		float height = cosf(heightAngleOffset * row) * radius;
		float width = sinf(heightAngleOffset * row) * radius;

		for (uint16_t col = 0; col < resolution; col++) {
			glm::vec3 normal = glm::normalize(glm::vec3(cosf(angleOffset * col) * width, height, sinf(angleOffset * col) * width));

			vertices.push_back(Vertex(
				glm::vec3(cosf(angleOffset * col) * width, height, sinf(angleOffset * col) * width),
				glm::vec3(1.0f, 1.0f, 1.0f),
				normal,
				glm::vec2((float)col / resolution, (float)height / resolution)));
		}
	}

	//Add bottom
	vertices.push_back(Vertex(glm::vec3(0.0f, -radius, 0.0f), glm::vec3(1.0f, 1.0f,1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));

	//Set Indices
	//Add Cap
	for (uint16_t i = 1; i <= resolution; i++) {
		if (i == 1) {
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(resolution);
		}
		else {
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i - 1);
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
					indices.push_back(rowEnd + resolution);
					indices.push_back(rowEnd);
					indices.push_back(j);
					indices.push_back(j + resolution);
					indices.push_back(rowEnd + resolution);
				}
				else {
					indices.push_back(j);
					indices.push_back((j - 1) + resolution);
					indices.push_back(j - 1);
					indices.push_back(j);
					indices.push_back(j + resolution);
					indices.push_back((j - 1) + resolution);
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
			indices.push_back(rowEnd);
			indices.push_back(rowStart);
		}
		else {
			indices.push_back(vertices.size() - 1);
			indices.push_back(rowStart + i - 1);
			indices.push_back(rowStart + i);
		}
	}

	if (vertexBuffer != nullptr && indexBuffer != nullptr) {
		UpdateVertexBuffer();
		UpdateIndexBuffer();
	}
}

void Mesh::GenerateLine(glm::vec3 point1, glm::vec3 point2)
{
	//Set vertices
	vertices.resize(3);

	vertices = {
		{point1, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{(point1 + point2) * 0.5f, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
		{point2, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0, 1, 2
	};

	if (vertexBuffer != nullptr && indexBuffer != nullptr) {
		UpdateVertexBuffer();
		UpdateIndexBuffer();
	}
}

void Mesh::LoadModel(const std::string modelPath) {

	vertices.clear();
	indices.clear();
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.textureCoordinate = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
				0.0f
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	if (vertexBuffer != nullptr && indexBuffer != nullptr) {
		UpdateVertexBuffer();
		UpdateIndexBuffer();
	}
}

#pragma endregion