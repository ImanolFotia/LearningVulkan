#pragma once

#include "renderer.hpp"

#include <vk/vk.hpp>


#include <vector>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
        using RenderPipelines = std::vector<vk::VulkanRenderPipeline>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        uint32_t Stage(const std::vector<Vertex> &, const MaterialInfo &) override;

        void Submit(const std::vector<Vertex>&, const MaterialInfo&) override;

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        framework::Window *m_pWindow;

        uint32_t m_pCurrentFrame;
        uint32_t m_pImageIndex;

        vk::VulkanData m_pVkData;

        RenderPipelines m_pRenderPipelines;
        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;
        VertexBuffers m_pVertexBuffers;
        vk::VulkanVertexInfo<6> m_pVertexInfo;

        uint32_t current_vertex_count = 0;
    };
}