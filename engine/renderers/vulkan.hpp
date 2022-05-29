#pragma once

#include "renderer.hpp"

#include <vk/vk.hpp>

#include <vector>
#include <list>

namespace engine
{
    class VulkanRenderer : public Renderer
    {
        using RenderPipelines = std::vector<vk::VulkanRenderPipeline>;
        using CommandPools = std::vector<VkCommandPool>;
        using CommandBuffers = std::vector<VkCommandBuffer>;
        using VertexBuffers = std::vector<vk::VulkanBuffer>;
        using IndexBuffers = std::vector<vk::VulkanBuffer>;
        using IndexType = uint32_t;

        using VertexContainer = std::vector<Vertex>;
        using IndexContainer = std::vector<IndexType>;

        using UniformBuffers = std::vector<vk::VulkanBuffer>;

    public:
        VulkanRenderer();

        ~VulkanRenderer();

        void Init(const char *appName, framework::Window &) override;

        uint32_t Submit(const std::vector<Vertex> &, const std::vector<IndexType>& indices, const MaterialInfo &) override;

        void Push(uint32_t) override;

        void PushCameraData(const CameraData& camData);

        void Begin() override;

        void End() override;

        void Flush() override;

        void Cleanup() override;

    private:
        vk::VulkanBuffer* createVertexBuffer();
        vk::VulkanBuffer* createIndexBuffer();
        vk::VulkanBuffer* createUniformBuffer(size_t);

        std::pair<vk::VulkanBuffer*, vk::VulkanBuffer*> getBuffers(const VertexContainer& vertices, const IndexContainer& indices);

        void createStagingBuffer(const std::vector<Vertex> &);
        void createStagingIndexBuffer(const std::vector<IndexType> &);
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();

        void updateUniforms();

    private:
        framework::Window *m_pWindow = nullptr;

        uint32_t m_pCurrentFrame = 0;
        uint32_t m_pImageIndex = 0;

        vk::VulkanData m_pVkData;

        RenderPipelines m_pRenderPipelines;
        CommandPools m_pCommandPools;
        CommandBuffers m_pCommandBuffers;

        VertexBuffers m_pVertexBuffers;
        IndexBuffers m_pIndexBuffers;

        UniformBuffers m_pUniformBuffers;
        VkDescriptorPool m_pDescriptorPool;
        std::vector<VkDescriptorSet> m_pDescriptorSets;

        vk::VulkanBuffer m_pStagingBuffer;
        vk::VulkanBuffer m_pStagingIndexBuffer;

        vk::VulkanVertexInfo<6> m_pVertexInfo;

        uint32_t current_vertex_count = 0;
        uint32_t current_index_count = 0;
        uint32_t allocations_count = 0;
        std::list<uint32_t> m_pCurrentFrameObjects;

        CameraData m_pCameraData{};
    };
}