
#include "pch.h"
#include "Image.h"
#include "TextureImages.h"
#include "VulkanManager.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 
#include "Buffer.h"

TextureImages* TextureImages::instance = nullptr;
TextureImages* TextureImages::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureImages();
	}

	return instance;
}
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
	//mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;


	if (!pixels) { throw std::runtime_error("failed to load texture image!"); }

	Buffer stagingBuffer;

	Buffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer);

	void* data;
	vkMapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory());
	stbi_image_free(pixels);


	Image::CreateImage(texWidth, texHeight, /*mipLevels, VK_SAMPLE_COUNT_1_BIT,*/ VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage); 		//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Image::TransitionImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	Image::CopyBufferToImage(stagingBuffer.GetBuffer(), textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	Image::TransitionImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBuffer(), nullptr);
	vkFreeMemory(VulkanManager::GetInstance()->GetLogicalDevice(), stagingBuffer.GetBufferMemory(), nullptr);
	//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, mipLevels);


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
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;//static_cast<float>(mipLevels / 2);
	samplerInfo.maxLod = 0.0f;//static_cast<float>(mipLevels);
	samplerInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(VulkanManager::GetInstance()->GetLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}

}

void TextureImages::CreateTextureImageView() {
	textureImageView = Image::CreateImageView(*textureImage.GetImage(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

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
