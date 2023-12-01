#include "ImageCreator.h"
#include "MemoryHelper.h"

#include <stdexcept>

ImageCreator::ImageCreator() {}

ImageCreator::ImageCreator(VkPhysicalDevice const &physicalDevice, VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, CommandWrapper &commandWrapper, BufferCreator const &bufferCreator)
	: physicalDevice(physicalDevice), device(device), queueFamilyIndices(queueFamilyIndices), commandWrapper(&commandWrapper), bufferCreator(bufferCreator) {}

ImageCreator::~ImageCreator() {}

void ImageCreator::createImage(uint32_t const width, uint32_t const height, VkFormat const format, VkImageTiling const imageTiling, VkImageUsageFlags const imageUsageFlags, VkMemoryPropertyFlags const memoryPropertyFlags, VkImage &image, VkDeviceMemory &imageMemory) const {

	//Creating the image create info struct, which will be passed as argument to the createImage call
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = imageTiling;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = imageUsageFlags;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	imageCreateInfo.queueFamilyIndexCount = 2;
	uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.transferFamilyIndex };
	imageCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	//Creating the memory allocate info struct, which will be passed as argument to the allocateMemory call
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = MemoryHelper::findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory");
	}

	//Binding the image and image memory
	vkBindImageMemory(device, image, imageMemory, 0);
}

void ImageCreator::create3DImage(uint32_t const width, uint32_t const height, uint32_t const depth, VkFormat const format, VkImageTiling const imageTiling, VkImageUsageFlags const imageUsageFlags, VkMemoryPropertyFlags const memoryPropertyFlags, VkImage &image, VkDeviceMemory &imageMemory) const {
	//Creating the image create info struct, which will be passed as argument to the createImage call
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_3D;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = depth;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = imageTiling;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = imageUsageFlags;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	imageCreateInfo.queueFamilyIndexCount = 2;
	uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.transferFamilyIndex };
	imageCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	//Creating the memory allocate info struct, which will be passed as argument to the allocateMemory call
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = MemoryHelper::findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory");
	}

	//Binding the image and image memory
	vkBindImageMemory(device, image, imageMemory, 0);
}

void ImageCreator::createImageView(VkImage const image, VkFormat const format, VkImageAspectFlags const aspectFlags, VkImageViewType const &viewType, VkImageView &imageView) const {

	//Creating the image view create info struct, which will be passed as argument to the createImageView call
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = viewType;
	imageViewCreateInfo.format = format;

	//Setting the sub resoureRange, which defines how the image is going to be used
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image view");
	}
}

void ImageCreator::createTextureImageView(VkImage const textureImage, VkImageView &textureImageView) const {
	createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, textureImageView);
}

void ImageCreator::createCloudTextureImageView(VkImage const textureImage, VkImageView &textureImageView) const {
	createImageView(textureImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_3D, textureImageView);
}

void ImageCreator::createTextureImage(char const *filePath, VkImage &textureImage, VkDeviceMemory &textureImageMemory) const {
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	uint32_t imageWidth;
	uint32_t imageHeight;

	//Loading the texture into the staging buffer
	bufferCreator.createTextureBuffer(filePath, stagingBuffer, stagingBufferMemory, imageWidth, imageHeight);

	//Creating the texture image
	createImage(imageWidth, imageHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	//Copying the buffer into the image and transitioning the image before and after into useful layouts
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, imageWidth, imageHeight, 1);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//Destroying the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ImageCreator::createCloudTextureImage(glm::vec4 values[], uint32_t const imageWidth, uint32_t const imageHeight, uint32_t const imageDepth, VkImage &textureImage, VkDeviceMemory &textureImageMemory) const {
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkDeviceSize imageSize = imageWidth * imageHeight * imageHeight * sizeof(values[0]);

	//Loading the texture into the staging buffer
	bufferCreator.create3DTextureBuffer(values, imageSize, stagingBuffer, stagingBufferMemory);

	//Creating the texture image
	//create3DImage(imageWidth, imageHeight, imageDepth, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
	create3DImage(imageWidth, imageHeight, imageDepth, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	//Copying the buffer into the image and transitioning the image before and after into useful layouts
	transitionImageLayout(textureImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, imageWidth, imageHeight, imageDepth);
	transitionImageLayout(textureImage, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//Destroying the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void ImageCreator::createTextureSampler(VkSampler &textureSampler) const {

	//Creating the sampler create info struct, which will be passed as argument to the createSampler call
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	/*samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;*/
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerCreateInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler");
	}
}

void ImageCreator::createCloudTextureSampler(VkSampler &textureSampler) const {
	//Creating the sampler create info struct, which will be passed as argument to the createSampler call
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerCreateInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create cloud texture sampler");
	}
}

void ImageCreator::transitionImageLayout(VkImage image, VkFormat const format, VkImageLayout const oldImageLayout, VkImageLayout const newImageLayout) const {
	VkCommandBuffer commandBuffer = commandWrapper->beginRecordingSingleUseTransferCommandBuffer();

	//Setting up the memory barrier used to transition the image
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //Need to change this maybe if exclusive gets implemented
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStageMask;
	VkPipelineStageFlags destinationStageMask;

	//Determining how to transition the image
	if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("Unsupported image layout transition");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStageMask, destinationStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	commandWrapper->endRecordingSingleUseTransferCommandBuffer(commandBuffer);
}

VkFormat ImageCreator::findDepthFormat() const {
	return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void ImageCreator::createComputTexture(uint32_t const width, uint32_t const height, VkImage &computeTextureImage, VkDeviceMemory &computeTextureImageMemory) const {
	createImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, computeTextureImage, computeTextureImageMemory);
}

void ImageCreator::createComputeTextureSampler(VkSampler &computeTextureSampler) const {
	//Creating the sampler create info struct, which will be passed as argument to the createSampler call
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerCreateInfo, nullptr, &computeTextureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler");
	}
}

void ImageCreator::createComputeTextureView(VkImage const computeTextureImage, VkImageView &computeTextureImageView) const {
	createImageView(computeTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, computeTextureImageView);
}

VkFormat ImageCreator::findSupportedFormat(std::vector<VkFormat> const &formatCandidates, VkImageTiling const imageTiling, VkFormatFeatureFlags formatFeatureFlags) const {

	//Checks for every format candidate if it fulfills the neccessary requirements
	for (VkFormat format : formatCandidates) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

		if (imageTiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & formatFeatureFlags) == formatFeatureFlags) {
			return format;
		}
		else if (imageTiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & formatFeatureFlags) == formatFeatureFlags) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format");
}

bool ImageCreator::hasStencilComponent(VkFormat const format) const {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void ImageCreator::copyBufferToImage(VkBuffer const buffer, VkImage image, uint32_t const imageWidth, uint32_t const imageHeight, uint32_t const imageDepth) const {
	VkCommandBuffer commandBuffer = commandWrapper->beginRecordingSingleUseTransferCommandBuffer();

	//Setting up what gets copied
	VkBufferImageCopy bufferImageCopy{};
	bufferImageCopy.bufferOffset = 0;
	bufferImageCopy.bufferRowLength = 0;
	bufferImageCopy.bufferImageHeight = 0;

	bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferImageCopy.imageSubresource.mipLevel = 0;
	bufferImageCopy.imageSubresource.baseArrayLayer = 0;
	bufferImageCopy.imageSubresource.layerCount = 1;

	bufferImageCopy.imageOffset = { 0,0,0 };
	bufferImageCopy.imageExtent = { imageWidth, imageHeight, imageDepth };

	//Submitting the copying of the buffer into the image
	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

	commandWrapper->endRecordingSingleUseTransferCommandBuffer(commandBuffer);
}
