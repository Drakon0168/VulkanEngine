#include "pch.h"
#include "EntityManager.h"

#include "VulkanManager.h"
#include "SwapChain.h"
#include "Image.h"
#pragma region Singleton

EntityManager* EntityManager::instance = nullptr;

EntityManager* EntityManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new EntityManager();
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

std::vector<std::shared_ptr<Material>> EntityManager::GetMaterials()
{
    return materials;
}

std::vector<std::shared_ptr<Mesh>> EntityManager::GetMeshes()
{
    return meshes;
}

#pragma endregion

#pragma region Initialization

void EntityManager::Init()
{
    LoadMaterials();

    LoadMeshes();

    int count = 0;

    //Setup entity list
    //  Add Materials and Mesh Lists
    for (std::shared_ptr<Material> material : materials) {
        entities.insert(std::pair <std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>>(material, std::vector<std::shared_ptr<Mesh>>()));
        count++;
    }
  
    //  Populate Mesh Lists
    for (std::shared_ptr<Mesh> mesh : meshes) {
        entities[mesh->GetMaterial()].push_back(mesh);
       
    }
    std::cout << count;
}

void EntityManager::LoadMeshes()
{
    meshes.resize(MeshTypes::MeshTypeCount);

    meshes[MeshTypes::Plane] = std::make_shared<Mesh>(materials[0]);
    meshes[MeshTypes::Plane]->GeneratePlane();

    meshes[MeshTypes::Cube] = std::make_shared<Mesh>(materials[1]);
    meshes[MeshTypes::Cube]->GenerateCube();

    meshes[MeshTypes::Sphere] = std::make_shared<Mesh>(materials[0]);
    meshes[MeshTypes::Sphere]->GenerateSphere(50);

    meshes[MeshTypes::Model] = std::make_shared<Mesh>(materials[1]);
    meshes[MeshTypes::Model]->LoadModel("models/room.obj");
}

void EntityManager::LoadMaterials()
{
    materials.push_back(std::make_shared<Material>("shaders/vert.spv", "shaders/frag.spv", "textures/frog.jpg"));
    materials.push_back(std::make_shared<Material>("shaders/vert.spv", "shaders/frag.spv", "textures/room.png"));
}

#pragma endregion

#pragma region Update

void EntityManager::Update()
{
    for (std::shared_ptr<Mesh> mesh : meshes) {
        mesh->UpdateInstanceBuffer();
    }
}

void EntityManager::Draw(uint32_t imageIndex, VkCommandBuffer* commandBuffer)
{
    if (vkResetCommandBuffer(*commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command buffer!");
    }

    //Setup command
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording Command Buffer!");
    }

    //Setup render pass
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = SwapChain::GetInstance()->GetRenderPass();
    renderPassBeginInfo.framebuffer = SwapChain::GetInstance()->GetFrameBuffers()[imageIndex];
    renderPassBeginInfo.renderArea.extent = SwapChain::GetInstance()->GetExtents();
    renderPassBeginInfo.renderArea.offset = { 0, 0 };

    std::array<VkClearValue, 2> clearColors = {};
    clearColors[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearColors[1].depthStencil = { 1.0f, 0 };

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
    renderPassBeginInfo.pClearValues = clearColors.data();

    //Setup commands
    vkCmdBeginRenderPass(*commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Begin Per Material Commands
    for (std::shared_ptr<Material> material : materials) {
        vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline());//Per material

        vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout(), 0, 1, &material->GetDescriptorSets()[imageIndex], 0, nullptr);//Per Material

        //Begin Per Mesh Commands
        for (std::shared_ptr<Mesh> mesh : entities[material]) {
            if (mesh->GetActiveInstanceCount() > 0) {
                VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(*commandBuffer, 0, 1, vertexBuffers, offsets);//Per mesh

                VkBuffer instanceBuffers[] = { mesh->GetInstanceBuffer()->GetBuffer() };
                vkCmdBindVertexBuffers(*commandBuffer, 1, 1, instanceBuffers, offsets);//Per mesh

                VkBuffer indexBuffers[] = { mesh->GetIndexBuffer()->GetBuffer() };
                vkCmdBindIndexBuffer(*commandBuffer, mesh->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);//Per mesh

                vkCmdDrawIndexed(*commandBuffer, static_cast<uint32_t>(mesh->GetIndices().size()), mesh->GetActiveInstanceCount(), 0, 0, 0);//Per mesh
            }
        }
    }

    vkCmdEndRenderPass(*commandBuffer);

    if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end Command Buffer!");
    }
}

#pragma endregion

#pragma region Memory Management

void EntityManager::CreateMaterialResources()
{
    for (size_t i = 0; i < materials.size(); i++) {
        materials[i]->Init();
        std::cout << materials.size();
    }
}

void EntityManager::CreateMeshResources()
{
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->Init();
    }
}

void EntityManager::Cleanup()
{
    //Cleanup Materials
    CleanupMaterials();

    //Cleanup Meshes
    CleanupMeshes();
}

void EntityManager::CleanupMaterials()
{
    for (size_t i = 0; i < materials.size(); i++) {
        materials[i]->Cleanup();
    }
}

void EntityManager::CleanupMeshes()
{
    for (size_t i = 0; i < meshes.size(); i++) {
        meshes[i]->Cleanup();
    }
}

#pragma endregion