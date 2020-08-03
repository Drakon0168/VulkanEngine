#include "pch.h"
#include "Image.h"

#include "VulkanManager.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 
#include "Buffer.h"
#pragma region Memory Management

void Image::Cleanup()
{
	vkDestroyImage(VulkanManager::GetInstance()->GetLogicalDevice(), image, nullptr);
	vkDestroyImageView(VulkanManager::GetInstance()->GetLogicalDevice(), view, nullptr);
	vkFreeMemory(VulkanManager::GetInstance()->GetLogicalDevice(), memory, nullptr);
}

#pragma endregion

#pragma region Accessors

VkImage* Image::GetImage()
{
	return &image;
}

void Image::SetImage(VkImage value)
{
	image = value;
}

VkImageView* Image::GetView()
{
	return &view;
}

VkDeviceMemory* Image::GetMemory()
{
	return &memory;
}

#pragma endregion

#pragma region Helper Methods

void Image::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Image& image)
{
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = usage;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(VulkanManager::GetInstance()->GetLogicalDevice(), &createInfo, nullptr, image.GetImage()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(VulkanManager::GetInstance()->GetLogicalDevice(), *image.GetImage(), &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = VulkanManager::GetInstance()->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(VulkanManager::GetInstance()->GetLogicalDevice(), &allocateInfo, nullptr, image.GetMemory()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory!");
	}

	vkBindImageMemory(VulkanManager::GetInstance()->GetLogicalDevice(), *image.GetImage(), *image.GetMemory(), 0);
}

void Image::CreateImageView(Image* image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = *image->GetImage();
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(VulkanManager::GetInstance()->GetLogicalDevice(), &createInfo, nullptr, image->GetView()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image view!");
	}
}

VkImageView Image::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(VulkanManager::GetInstance()->GetLogicalDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image view!");
	}

	return imageView;
}

void Image::TransitionImageLayout(Image image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommand();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = *image.GetImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::runtime_error("Unsupported Layput Transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	CommandBuffer::EndSingleTimeCommand(commandBuffer);
}

void Image::CopyBufferToImage(VkBuffer buffer, Image image, uint32_t imageWidth, uint32_t imageHeight)
{
	VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommand();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		imageWidth,
		imageHeight,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, *image.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	CommandBuffer::EndSingleTimeCommand(commandBuffer);
}

VkFormat Image::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(VulkanManager::GetInstance()->GetPhysicalDevice(), format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void Image::LoadAll() {
	LoadTexture("textures/room.jpg");
}
void Image::LoadTexture(const std::string texturePath){


	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	//mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;


	if (!pixels) { throw std::runtime_error("failed to load texture image!"); }

	Buffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	Buffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory());
	stbi_image_free(pixels);

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	if ((vkCreateImage(VulkanManager::GetInstance()->GetLogicalDevice(), &imageInfo, nullptr, &textureImage)) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");  }
	//createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory); 		//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	//copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	//vkDestroyBuffer(device, stagingBuffer, nullptr);
	//vkFreeMemory(device, stagingBufferMemory, nullptr);
	//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, mipLevels);
	
}
#pragma endregion