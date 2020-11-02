#pragma once

#include "pch.h"
#include "Image.h"
class TextureImages
{
private:
	VkImage image;
	VkImageView view;
	VkDeviceMemory memory;
	VkImageView textureImageView;
	Image textureImage;
	VkDeviceMemory textureImageMemory;

	uint32_t mipLevels;
	VkSampler textureSampler;

	
public:
#pragma region Singleton 
	//not anymore!
#pragma endregion
#pragma region Memory Management

	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layers = 1);
	void LoadTexture(const std::string texturePath);
	void LoadCubeMap(const std::string texturePath);

	void LoadAll();

	void CreateTextureImageView();

	void CreateTextureImageViewCube();

	void CreateTextureSampler();

	void Cleanup();
	
#pragma endregion

#pragma region Accessors
	VkImage TextureImageImage();

	Image GetTextureImage();
	
	VkDeviceMemory TextureImageMemory();

	VkImageView GetTextureImageView();

	VkSampler GetSampler();
#pragma endregion
};