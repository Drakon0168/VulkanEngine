#pragma once

#include "pch.h"

class Image
{
private:
	VkImage image;
	VkImageView view;
	VkDeviceMemory memory;

public:

#pragma region Memory Management

	/// <summary>
	/// Cleans up image resources
	/// </summary>
	void Cleanup();

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the VkImage associated with this image
	/// </summary>
	/// <returns>The VkImage</returns>
	VkImage* GetImage();

	/// <summary>
	/// Sets the VkImage associated with this image to the specified value
	/// </summary>
	/// <param name="value">The value to set the VkImage to</param>
	void SetImage(VkImage value);

	/// <summary>
	/// Returns the VkImageView associated with this image
	/// </summary>
	/// <returns>The VkImageView</returns>
	VkImageView* GetView();

	/// <summary>
	/// Returns the DeviceMemory associated with this image
	/// </summary>
	/// <returns></returns>
	VkDeviceMemory* GetMemory();

#pragma endregion

#pragma region Helper Methods

	/// <summary>
	/// Creates and Allocates an image to memory
	/// </summary>
	/// <param name="width">The width in pixels of the image</param>
	/// <param name="height">The height in pixels of the image</param>
	/// <param name="format">The pixel format of the image data</param>
	/// <param name="tiling">The tiling format of the image</param>
	/// <param name="usage">The usage type of the image</param>
	/// <param name="properties">The memory properties of the image</param>
	/// <param name="image">The image that is being allocated</param>
	static void CreateImage(uint32_t mipLevels, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, Image& image, uint32_t layers = 1, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);

	/// <summary>
	/// Creates an image view for the supplied image
	/// </summary>
	/// <param name="image">The image to attach the image view to</param>
	/// <param name="format">The format of the image</param>
	/// <param name="aspectFlags">The aspect flag defining the use of the image</param>
	static void CreateImageView(Image* image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	/// <summary>
	/// Creates an image view forr the specified image
	/// </summary>
	/// <param name="image">The image to create the image view for</param>
	/// <param name="format">The format of the image</param>
	/// <param name="aspectFlags">The aspect flag defining the use of the image</param>
	/// <returns>The image view that was created</returns>
	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layers = 1);

	/// <summary>
	/// Changes the layout of the image
	/// </summary>
	/// <param name="image">The image to change the layout of</param>
	/// <param name="oldLayout">The layout to transition from</param>
	/// <param name="newLayout">The layout to transition to</param>
	static void TransitionImageLayout(Image image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layers = 1);

	/// <summary>
	/// Copies the data stored in a buffer to an image
	/// </summary>
	/// <param name="buffer">The buffer to copy the data from</param>
	/// <param name="image">The image to copy the data to</param>
	/// <param name="imageWidth">The width of the image</param>
	/// <param name="imageHeight">The height of the image</param>
	static void CopyBufferToImage(VkBuffer buffer, Image image, uint32_t imageWidth, uint32_t imageHeight, uint32_t layers = 1, VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	/// <summary>
	/// Checks the physical device for format support
	/// </summary>
	/// <param name="candidates">Potential formats to be checked</param>
	/// <param name="tiling">Tiling formats to check for</param>
	/// <param name="features">Features to check for</param>
	/// <returns>A format that has the required features and is supported by the physical device</returns>
	static  VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


	 void LoadTexture(const std::string texturePath);

	void LoadAll();
#pragma endregion
};