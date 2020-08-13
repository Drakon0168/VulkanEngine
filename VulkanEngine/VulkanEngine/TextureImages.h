#pragma once

#include "pch.h"

class TextureImages
{
private:
	static TextureImages* instance;
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
	static TextureImages* GetInstance();
#pragma endregion
#pragma region Memory Management

	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void LoadTexture(const std::string texturePath);

	void LoadAll();

	void CreateTextureImageView();

	void CreateTextureSampler();
	
#pragma endregion

#pragma region Accessors
	VkImage TextureImageImage();

	Image GetTextureImage();
	
	VkDeviceMemory TextureImageMemory();

	VkImageView GetTextureImageView();

	VkSampler GetSampler();
#pragma endregion


};