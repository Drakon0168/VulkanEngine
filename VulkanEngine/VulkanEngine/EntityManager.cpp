#include "pch.h"
#include "EntityManager.h"

#include "DebugManager.h"
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
/*
std::vector<std::shared_ptr<TextureImages>> EntityManager::GetTextureImages()
{
    for (std::shared_ptr<Material> material : materials) {
        tImages.push_back(std::shared_ptr<TextureImages>(material.get()->GetTImage()));
    }
    return tImages;
}
*/
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

    meshes[MeshTypes::Skybox] = std::make_shared<Mesh>(materials[2]);
    meshes[MeshTypes::Skybox]->GenerateCube();

    meshes[MeshTypes::WireCube] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::WireCube]->GenerateCube();

    meshes[MeshTypes::WireSphere] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::WireSphere]->GenerateSphere(10);

    meshes[MeshTypes::Line] = std::make_shared<Mesh>(materials[3]);
    meshes[MeshTypes::Line]->GenerateLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void EntityManager::LoadMaterials()
{
    
    std::vector<std::vector<VkVertexInputAttributeDescription>> attributeDescriptions;
    attributeDescriptions.push_back(Vertex::GetAttributeDescriptions(0, 0));
    attributeDescriptions.push_back(TransformData::GetAttributeDescriptions(attributeDescriptions[0].size(), 1));

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    bindingDescriptions.push_back(Vertex::GetBindingDescription(0));
    bindingDescriptions.push_back(TransformData::GetBindingDescription(bindingDescriptions.size()));

    materials.push_back(std::make_shared<Material>("shaders/vert.spv", "shaders/frag.spv", false, attributeDescriptions, bindingDescriptions, "textures/frog.jpg"));
    materials.push_back(std::make_shared<Material>("shaders/vert.spv", "shaders/frag.spv", false, attributeDescriptions, bindingDescriptions, "textures/room.png"));
    materials.push_back(std::make_shared<Material>("shaders/SkyVert.spv", "shaders/SkyFrag.spv", false, attributeDescriptions, bindingDescriptions, "textures/Skybox/", 'S'));
    materials.push_back(std::make_shared<Material>("shaders/SkyVert.spv", "shaders/SkyFrag.spv", false, attributeDescriptions, bindingDescriptions, "textures/Skybox/", 'S'));

    //TODO: Find a better way of doing this that will work for multiple types of inputs
    std::vector<VkVertexInputAttributeDescription> attributeDescription(1);
    attributeDescription[0].binding = 2;
    attributeDescription[0].location = 8;
    attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[0].offset = 0;
    attributeDescriptions.push_back(attributeDescription);

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 2;
    bindingDescription.stride = sizeof(glm::vec3);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    bindingDescriptions.push_back(bindingDescription);
    materials.push_back(std::make_shared<Material>("shaders/DebugVert.spv", "shaders/DebugFrag.spv", true, attributeDescriptions, bindingDescriptions, "textures/room.png"));
}

#pragma endregion

#pragma region Update

void EntityManager::Update()
{
    for (std::shared_ptr<Mesh> mesh : meshes) {
        if (mesh->GetActiveInstanceCount() > 0)
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
                //TODO: Make sure this changes based on the attributes used by the current material
                VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers(*commandBuffer, 0, 1, vertexBuffers, offsets);//Per mesh

                VkBuffer instanceBuffers[] = { mesh->GetInstanceBuffer()->GetBuffer() };
                vkCmdBindVertexBuffers(*commandBuffer, 1, 1, instanceBuffers, offsets);//Per mesh

                //Add the color instance buffer only for the debug shapes
                VkBuffer colorBuffer[1];
                if (DebugManager::GetInstance()->GetInstanceBuffers().count(mesh) != 0) {
                     colorBuffer[0] = DebugManager::GetInstance()->GetInstanceBuffers()[mesh]->GetBuffer();
                    vkCmdBindVertexBuffers(*commandBuffer, 2, 1, colorBuffer, offsets);
                }

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