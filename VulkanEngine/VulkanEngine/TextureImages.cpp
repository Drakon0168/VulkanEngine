
#include "pch.h"
#include "Image.h"
#include "TextureImages.h"
#include "VulkanManager.h"

//STB Includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Buffer.h"

void TextureImages::LoadAll() {
	LoadTexture("textures/room.jpg");
}
void TextureImages::LoadTexture(const std::string texturePath) {

	int texWidth, texHeight, texChannels;
	if (texturePath.c_str() == NULL) {
		throw std::runtime_error("path is null");
	}
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	// mipLevels = 0;


	if (!pixels) { throw std::runtime_error("failed to load texture image!"); }

	Buffer stagingBuffer;

	Buffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory());
	stbi_image_free(pixels);


	Image::CreateImage(mipLevels, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage); 		//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Image::TransitionImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	Image::CopyBufferToImage(stagingBuffer.GetBuffer(), textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	textureImageMemory = *textureImage.GetMemory();
	vkDestroyBuffer(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBuffer(), nullptr);
	vkFreeMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), nullptr);
	GenerateMipmaps(*textureImage.GetImage(), VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);

}

void TextureImages::LoadCubeMap(const std::string texturePath)
{
	int texWidth, texHeight, texChannels;
	if (texturePath.c_str() == NULL) {
		throw std::runtime_error("path is null");
	}
	stbi_uc* pixels[6];
	std::vector<stbi_uc*> pixelVector;
	std::string file = texturePath;
	stbi_uc* pixel = stbi_load((file + "SkyboxLR.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	// stbi_uc* pixel = stbi_load((file + "Skybox.png").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[0] = stbi_load((file + "Right.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[1] = stbi_load((file + "Left.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[2] = stbi_load((file + "Top.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[3] = stbi_load((file + "Bot.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[5] = stbi_load((file + "Back.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	pixels[4] = stbi_load((file + "Front.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	

	
	// stbi_uc* pixel = stbi_load((file + "AllSky.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
	VkDeviceSize layerSize = imageSize / 6;
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	mipLevels = 1;


	if (!pixel) { throw std::runtime_error("failed to load texture image! jjjjj"); }
	for (int i = 0; i < 6; i++) {
		if (!pixels[i]) { throw std::runtime_error("failed to load texture image! hhhh"); }
	}

	Buffer stagingBuffer;

	Buffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void *data;
	// void* dataOffset = &data + layerSize;
	//VkResult res = 
	vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, imageSize, 0, &data);
	for (uint8_t i = 0; i < 6; ++i) {
		// since an int is 32-bit, we need to divide layerSize by 4 to get the number of bytes as our offset
		memcpy(static_cast<int*>(data) + (layerSize / 4) * i, pixels[i], static_cast<size_t>(layerSize));
		// break;
	}
	vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory());
	stbi_image_free(pixel);
	for (size_t i = 0; i < 6; ++i) {
		stbi_image_free(pixels[i]);
	}


	Image::CreateImage(mipLevels, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, 6);  
	Image::TransitionImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 6);
	Image::CopyBufferToImage(stagingBuffer.GetBuffer(), textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 6);
	textureImageMemory = *textureImage.GetMemory();
	vkDestroyBuffer(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBuffer(), nullptr);
	vkFreeMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), nullptr);
	GenerateMipmaps(*textureImage.GetImage(), VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels, 6);

}

void TextureImages::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layers) {
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(VulkanManager::GetInstance()->GetPhysicalDevice(), imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommand();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	CommandBuffer::EndSingleTimeCommand(commandBuffer);

}
void TextureImages::CreateTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	if (mipLevels > 1) {
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = static_cast<float>(mipLevels / 2);
		samplerInfo.maxLod = static_cast<float>(mipLevels);
	}
	samplerInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(VulkanManager::GetInstance()->GetLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}

}
void TextureImages::Cleanup() {
	vkDestroySampler(VulkanManager::GetInstance()->GetLogicalDevice(),textureSampler, nullptr);

	vkDestroyImage(VulkanManager::GetInstance()->GetLogicalDevice(), *textureImage.GetImage(), nullptr);
	vkDestroyImageView(VulkanManager::GetInstance()->GetLogicalDevice(), textureImageView, nullptr);
	vkFreeMemory(VulkanManager::GetInstance()->GetLogicalDevice(), textureImageMemory, nullptr);
}

void TextureImages::CreateTextureImageView() {
	textureImageView = Image::CreateImageView(*textureImage.GetImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

void TextureImages::CreateTextureImageViewCube()
{
	textureImageView = Image::CreateImageView(*textureImage.GetImage(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_VIEW_TYPE_CUBE, 6);
}

Image TextureImages::GetTextureImage() {
	return textureImage;
}

VkImageView TextureImages::GetTextureImageView() {
	return textureImageView;
}
VkImage TextureImages::TextureImageImage() {
	return *textureImage.GetImage();
}

VkDeviceMemory TextureImages::TextureImageMemory() {
	
	return textureImageMemory;
}
VkSampler TextureImages::GetSampler() {
	return textureSampler;
}
