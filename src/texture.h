#pragma once

#include "devloc.h"
#include "cmdloc.h"
#include "create.h"
#include "image.h"

class Texture : public Image {
    public:
        Texture(std::string filename) {
            createImage(filename);
            createImageView();
            createSampler();
        }

        ~Texture() {
            DevLoc::device()->destroy(textureSampler);
            DevLoc::device()->destroy(textureImageView);
            DevLoc::device()->destroy(textureImage);
            DevLoc::device()->free(textureImageMemory);
        }

        VkImageView& view() {
            return textureImageView;
        }

        VkSampler& sampler() {
            return textureSampler;
        }

    private:
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        void createImage(std::string filename) {
            int texWidth, texHeight, texChannels;
            stbi_uc* pixels = stbi_load(filename.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;

            if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            create::buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            DevLoc::device()->map(stagingBufferMemory, imageSize, data);
            /**/memcpy(data, pixels, static_cast<size_t>(imageSize));
            DevLoc::device()->unmap(stagingBufferMemory);

            stbi_image_free(pixels);

            create::image(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

            CmdLoc::cmd()->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            CmdLoc::cmd()->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            CmdLoc::cmd()->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            DevLoc::device()->destroy(stagingBuffer);
            DevLoc::device()->free(stagingBufferMemory);
        }

        void createImageView() {
            textureImageView = create::imageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
        }

        void createSampler() {
            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = 16;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

            DevLoc::device()->create(samplerInfo, textureSampler);
        }
};
