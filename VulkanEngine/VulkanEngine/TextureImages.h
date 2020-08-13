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


	VkSampler textureSampler;

	
public:
#pragma region Singleton 
	static TextureImages* GetInstance();
#pragma endregion
#pragma region Memory Management

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