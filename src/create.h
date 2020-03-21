#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "devloc.h"
#include "cmdloc.h"
#include "vertex.h"

namespace create {
    void buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        DevLoc::device()->create(bufferInfo, buffer);

        VkMemoryRequirements memRequirements;
        DevLoc::device()->get(buffer, memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = DevLoc::device()->find(memRequirements.memoryTypeBits, properties);

        DevLoc::device()->allocate(allocInfo, bufferMemory);

        DevLoc::device()->bind(buffer, bufferMemory);
    }

    void vertexBuffer(std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        create::buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        DevLoc::device()->map(stagingBufferMemory, bufferSize, data);
        /**/memcpy(data, vertices.data(), (size_t) bufferSize);
        DevLoc::device()->unmap(stagingBufferMemory);

        create::buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

        CmdLoc::cmd()->copyBuffer(stagingBuffer, buffer, bufferSize);

        DevLoc::device()->destroy(stagingBuffer);
        DevLoc::device()->free(stagingBufferMemory);
    }

    void indexBuffer(std::vector<uint32_t>& indeces, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkDeviceSize bufferSize = sizeof(indeces[0]) * indeces.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        create::buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        DevLoc::device()->map(stagingBufferMemory, bufferSize, data);
        /**/memcpy(data, indeces.data(), (size_t) bufferSize);
        DevLoc::device()->unmap(stagingBufferMemory);

        create::buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

        CmdLoc::cmd()->copyBuffer(stagingBuffer, buffer, bufferSize);

        DevLoc::device()->destroy(stagingBuffer);
        DevLoc::device()->free(stagingBufferMemory);
    }

    VkImageView imageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, int layerCount=1, VkImageViewType viewType=VK_IMAGE_VIEW_TYPE_2D) {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = viewType;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = layerCount;

        VkImageView imageView;
        DevLoc::device()->create(viewInfo, imageView);

        return imageView;
    }

    void cubemap(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.arrayLayers = 6;
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        DevLoc::device()->create(imageInfo, image);

        VkMemoryRequirements memRequirements;
        DevLoc::device()->get(image, memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = DevLoc::device()->find(memRequirements.memoryTypeBits, properties);

        DevLoc::device()->allocate(allocInfo, imageMemory);

        DevLoc::device()->bind(image, imageMemory);
    }

    void image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        DevLoc::device()->create(imageInfo, image);

        VkMemoryRequirements memRequirements;
        DevLoc::device()->get(image, memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = DevLoc::device()->find(memRequirements.memoryTypeBits, properties);

        DevLoc::device()->allocate(allocInfo, imageMemory);

        DevLoc::device()->bind(image, imageMemory);
    }
}