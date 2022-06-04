#include <framework/window.hpp>

#include "vulkan.hpp"
#include <framework/common.hpp>

/**
 * @brief Implementation of the Vulkan renderer public API
 *
 */

namespace engine
{

    VulkanRenderer::VulkanRenderer()
    {
    }
    VulkanRenderer::~VulkanRenderer()
    {
    }
    void VulkanRenderer::Init(const char *appName, framework::Window &window)
    {
        m_pWindow = &window;
        vk::createInstance(appName, m_pVkData);
        vk::setupDebugMessenger(m_pVkData.instance);
        vk::createSurface(m_pVkData, window.getWindow());
        vk::pickPhysicalDevice(m_pVkData);
        vk::createLogicalDevice(m_pVkData);
        vk::createSwapChain(m_pVkData, window.getWindow());
        vk::createImageViews(m_pVkData);

        RenderPassInfo renderPassInfo;
        renderPassInfo.numDescriptors = 6;
        renderPassInfo.size = sizeof(Vertex);
        renderPassInfo.vertexLayout = {{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
                                       {VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)},
                                       {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
                                       {VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)},
                                       {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)},
                                       {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent)}};

        addRenderpass(renderPassInfo);
        /*m_pRenderPasses.emplace_back();
        m_pRenderPasses.back().renderPipelines.emplace_back();
        vk::createRenderPass(m_pVkData, m_pRenderPasses.back());

        m_pVertexInfo.attributeDescriptions =
            vk::getAttributeDescriptions<6>(0,
                                            {{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
                                             {VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
                                             {VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent)}});

        m_pVertexInfo.bindingDescription = vk::getBindingDescription<Vertex>();
        vk::createDescriptorSetLayout(m_pVkData, m_pRenderPasses.back().renderPipelines.back());

        vk::createGraphicsPipeline<MeshPushConstant>(m_pVkData, m_pRenderPasses.back(), 0, m_pVertexInfo);
        vk::createFramebuffers(m_pVkData,  m_pRenderPasses.back());
*/
        m_pCommandPools.emplace_back();

        vk::createCommandPool(m_pVkData, m_pCommandPools.back());

        pCreateVertexBuffer();
        pCreateIndexBuffer();
        pCreateUniformBuffers();
        pCreateTextureBuffer();

        pCreateDescriptorPool();
        pCreateDescriptorSets();

        vk::createCommandBuffers(m_pVkData, m_pCommandPools.back(), m_pCommandBuffers);
        vk::createSyncObjects(m_pVkData);
    }

    uint32_t VulkanRenderer::addRenderpass(RenderPassInfo renderPassInfo)
    {
        m_pRenderPasses[renderpass_id] = {};

        m_pRenderPasses.at(renderpass_id).renderPipelines.emplace_back();

        vk::createRenderPass(m_pVkData, m_pRenderPasses.at(renderpass_id));

        m_pVertexInfo.attributeDescriptions =
            vk::getAttributeDescriptions(0, {{VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
                                             {VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
                                             {VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)},
                                             {VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, bitangent)}});

        m_pVertexInfo.bindingDescription = vk::getBindingDescription(renderPassInfo.size);

        vk::createDescriptorSetLayout(m_pVkData, m_pRenderPasses.at(renderpass_id).renderPipelines.back());

        vk::createGraphicsPipeline<MeshPushConstant>(m_pVkData, m_pRenderPasses.at(renderpass_id), 0, m_pVertexInfo);

        vk::createFramebuffers(m_pVkData, m_pRenderPasses.at(renderpass_id));

        return ++renderpass_id;
    }

    Renderer::ObjectDataId VulkanRenderer::Submit(const std::vector<Vertex> &vertices, std::vector<IndexType> &indices, const MaterialInfo &, bool group = true)
    {
        for (auto &index : indices)
        {
            index += current_vertex_count;
        }
        auto [buffer, indexBuffer] = pGetBuffers(vertices, indices);

        pCreateStagingBuffer(vertices);
        pCreateStagingIndexBuffer(indices);

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingBuffer.buffer, buffer->buffer, vertices.size() * sizeof(Vertex), current_vertex_count * sizeof(Vertex));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", vertices.size(), " vertices, of size ", vertices.size() * sizeof(Vertex), " bytes, at offset ", current_vertex_count * sizeof(Vertex), " to local buffer");

        vk::copyBuffer(m_pVkData, m_pCommandPools.back(), m_pStagingIndexBuffer.buffer, indexBuffer->buffer, indices.size() * sizeof(IndexType), current_index_count * sizeof(IndexType));

        vkDestroyBuffer(m_pVkData.logicalDevice, m_pStagingIndexBuffer.buffer, nullptr);
        vkFreeMemory(m_pVkData.logicalDevice, m_pStagingIndexBuffer.deviceMemory, nullptr);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "copied ", indices.size(), " indices, of size ", indices.size() * sizeof(IndexType), " bytes, at offset ", current_index_count * sizeof(IndexType), " to local buffer");
        uint16_t curr_vertex_offset = current_vertex_count;
        uint16_t curr_index_offset = current_index_count;
        current_vertex_count += vertices.size();
        current_index_count += indices.size();

        buffer->allocatedVertices += vertices.size();
        indexBuffer->allocatedVertices += indices.size();

        m_pObjectData.emplace_back(vertices.size(), indices.size(), curr_vertex_offset, curr_index_offset, m_pVertexBuffers.size() - 1, m_pIndexBuffers.size() - 1, group);
        return std::prev(m_pObjectData.end());
    }

    void VulkanRenderer::Push(Renderer::ObjectDataId object_id)
    {
        m_pCurrentCommandQueue.push_back(object_id);
    }

    void VulkanRenderer::PushCameraData(const ShaderData &camData)
    {
        m_pCameraData = camData;
    }

    void VulkanRenderer::Begin()
    {
        m_pImageIndex = vk::prepareSyncObjects<MeshPushConstant>(m_pVkData,
                                                                 m_pWindow->getWindow(),
                                                                 m_pCurrentFrame,
                                                                 m_pRenderPasses.at(DefaultRenderPass),
                                                                 m_pVertexInfo);
        if (m_pImageIndex == -1)
            return;
        if (m_pImageIndex >= vk::MAX_FRAMES_IN_FLIGHT)
            m_pImageIndex = vk::MAX_FRAMES_IN_FLIGHT - 1;

        m_pFrame.FrameIndex(m_pCurrentFrame);
        m_pFrame.CurrentImage(m_pImageIndex);

        m_pFrame.CommandBuffer(&m_pCommandBuffers.at(m_pCurrentFrame));
        m_pFrame.UniformBuffer(&m_pUniformBuffers.at(m_pCurrentFrame));

        m_pFrame.SyncObjects(&m_pVkData.syncObjects.at(m_pCurrentFrame));

        // std::cout << "m_pImageIndex " << m_pImageIndex << std::endl;

        pUpdateUniforms();
        vkResetCommandBuffer(m_pFrame.CommandBuffer(), 0);

        vk::recordCommandBuffer(m_pFrame.CommandBuffer(), m_pImageIndex);
        vk::createRenderPassInfo(m_pImageIndex, m_pVkData, m_pRenderPasses.at(DefaultRenderPass));
        vk::beginRenderPass(m_pFrame.CommandBuffer(), m_pRenderPasses.at(DefaultRenderPass));
    }

    void VulkanRenderer::End()
    {
        if (m_pImageIndex == -1)
            return;

        vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
        vk::endRecording(m_pFrame.CommandBuffer());

        VkSemaphore signalSemaphores[] = {m_pFrame.SyncObjects().renderFinishedSemaphores};

        vk::Sync(m_pVkData, m_pFrame.CommandBuffer(), m_pCurrentFrame);

        bool should_recreate_swapchain = vk::Present(m_pVkData, signalSemaphores, m_pImageIndex);

        if (should_recreate_swapchain)
            vk::recreateSwapChain<MeshPushConstant>(m_pVkData, m_pWindow->getWindow(), m_pRenderPasses.at(DefaultRenderPass), m_pVertexInfo);

        m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::Flush()
    {
        if (m_pImageIndex == -1)
            return;

        if (m_pRenderPasses.at(DefaultRenderPass).renderPipelines.at(DefaultRenderPass).graphicsPipeline == NULL)
            std::cout << "pipeline is null\n";

        vk::bindPipeline(m_pRenderPasses.at(DefaultRenderPass).renderPipelines.at(DefaultRenderPass), m_pFrame.CommandBuffer());
        vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), m_pVertexBuffers[0].buffer);
        vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), m_pIndexBuffers[0].buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pRenderPasses.at(DefaultRenderPass).renderPipelines.back().pipelineLayout, 0, 1, &m_pDescriptorSets[m_pCurrentFrame], 0, nullptr);

        int num_indices = 0;
        int curr_offset = 0;
        int curr_indices = 0;

        /*m_pCurrentCommandQueue.sort([](auto first, auto second){
            return first->index_offset < second->index_offset;
        });*/
        bool prev_group = false;
        for (auto &command : m_pCurrentCommandQueue)
        {
            /*if (command->index_offset == num_indices && command->group != true)
            {
                num_indices += command->num_indices;
                curr_indices += command->num_indices;
                continue;
            }
            else
            {*/
            /*vk::drawIndexed(m_pFrame.CommandBuffer(), curr_indices, 1, curr_offset, 0, 0);
            curr_indices = 0;
            curr_offset = command->index_offset;
            num_indices = curr_offset + command->num_indices;
            curr_indices = curr_offset + command->num_indices;
            */
            //}
            vkCmdPushConstants(m_pFrame.CommandBuffer(), m_pRenderPasses.at(DefaultRenderPass).renderPipelines.back().pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstant), &command->push_constant);

            vk::drawIndexed(m_pFrame.CommandBuffer(), command->num_indices, 1, command->index_offset, 0, 0);
        }
        /*
                if (curr_indices > 0)
                    vk::drawIndexed(m_pFrame.CommandBuffer(), curr_indices, 1, curr_offset, 0, 0);
        */
        m_pCurrentCommandQueue.clear();
    }

    void VulkanRenderer::Cleanup()
    {
        vkDeviceWaitIdle(m_pVkData.logicalDevice);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(m_pVkData.logicalDevice, m_pUniformBuffers[i].buffer, nullptr);
            vkFreeMemory(m_pVkData.logicalDevice, m_pUniformBuffers[i].deviceMemory, nullptr);
        }

        vkDestroyDescriptorPool(m_pVkData.logicalDevice, m_pDescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(m_pVkData.logicalDevice, m_pRenderPasses.at(DefaultRenderPass).renderPipelines.back().descriptorSetLayout, nullptr);

        vk::cleanupSyncObjects(m_pVkData);
        vk::cleanCommandPool(m_pVkData, m_pCommandPools[0]);

        for (auto &buffer : m_pVertexBuffers)
        {
            vk::destroyVertexBuffer(m_pVkData, buffer.buffer);

            vkFreeMemory(m_pVkData.logicalDevice, buffer.deviceMemory, nullptr);
        }

        for (auto &buffer : m_pIndexBuffers)
        {
            vk::destroyVertexBuffer(m_pVkData, buffer.buffer);

            vkFreeMemory(m_pVkData.logicalDevice, buffer.deviceMemory, nullptr);
        }
        /*
                for (auto &allocations : m_pMemoryAllocations)
                {
                    vkFreeMemory(m_pVkData.logicalDevice, allocations.second.deviceMemory, nullptr);
                }*/

        for (auto &pass : m_pRenderPasses)
        {
            vk::cleanupRenderPass(m_pVkData, pass.second.renderPass);
            for (auto &pipeline : pass.second.renderPipelines)
                vk::destroyGraphicsPipeline(m_pVkData, pipeline);
        }
        vk::cleanup(m_pVkData);
    }
}