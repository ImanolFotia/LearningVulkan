#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "render_pass.hpp"

namespace vk
{

    static void createFramebuffers(VulkanData& vk_data, VulkanRenderPass& renderPass)
    {
        vk_data.swapChainFramebuffers.resize(vk_data.swapChainImageViews.size());

        for (size_t i = 0; i < vk_data.swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                vk_data.swapChainImageViews[i],
                vk_data.swapChainDepthTexture.imageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass.renderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = vk_data.swapChainExtent.width;
            framebufferInfo.height = vk_data.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vk_data.logicalDevice, &framebufferInfo, nullptr, &vk_data.swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
}