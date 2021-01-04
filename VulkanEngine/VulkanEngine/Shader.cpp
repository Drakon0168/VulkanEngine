#include "pch.h"
#include "Shader.h"

#include "DebugManager.h"
#include "Mesh.h"

#pragma region Constructor

Shader::Shader(std::string vertexPath, std::string fragmentPath, size_t textureCount, std::vector<std::shared_ptr<Buffer>> resources)
{
	this->fragmentPath = fragmentPath;
	this->vertexPath = vertexPath;
	this->textureCount = textureCount;
	this->resources = resources;
}

#pragma endregion

#pragma region Accessors

std::string Shader::GetFragmentPath()
{
	return fragmentPath;
}

void Shader::SetFragmentPath(std::string value)
{
	fragmentPath = value;
}

std::string Shader::GetVertexPath()
{
	return vertexPath;
}

void Shader::SetVertexPath(std::string value)
{
	vertexPath = value;
}

size_t Shader::GetTextureCount()
{
	return textureCount;
}

void Shader::SetTextureCount(size_t value)
{
	textureCount = value;
}

std::vector<std::shared_ptr<Buffer>> Shader::GetResources()
{
	return resources;
}

void Shader::SetResources(std::vector<std::shared_ptr<Buffer>> value)
{
	resources = value;
}

#pragma endregion

#pragma region Bindings

void Shader::BindResources(VkCommandBuffer* commandBuffer, std::shared_ptr<Mesh> mesh)
{
	//TODO: Make sure this changes based on the attributes used by the current material
	VkDeviceSize offsets[] = { 0 };
	VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
	
	vkCmdBindVertexBuffers(*commandBuffer, 0, 1, vertexBuffers, offsets);//Per mesh

	VkBuffer instanceBuffers[] = { mesh->GetInstanceBuffer()->GetBuffer() };
	vkCmdBindVertexBuffers(*commandBuffer, 1, 1, instanceBuffers, offsets);//Per mesh

	VkBuffer indexBuffers[] = { mesh->GetIndexBuffer()->GetBuffer() };
	vkCmdBindIndexBuffer(*commandBuffer, mesh->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);//Per mesh

	for (int i = 0; i < resources.size(); i++) {
		VkBuffer buffer[1] = { resources[i]->GetBuffer() };
		vkCmdBindVertexBuffers(*commandBuffer, 2 + i, 1, buffer, offsets);
	}

	//Add the color instance buffer only for the debug shapes
	/*VkBuffer colorBuffer[1];
	if (DebugManager::GetInstance()->GetInstanceBuffers().count(mesh) != 0) {
		colorBuffer[0] = DebugManager::GetInstance()->GetInstanceBuffers()[mesh]->GetBuffer();
		vkCmdBindVertexBuffers(*commandBuffer, 2, 1, colorBuffer, offsets);
	}*/
}

#pragma endregion