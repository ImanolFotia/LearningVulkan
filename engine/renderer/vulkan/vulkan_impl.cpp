#include "vulkan.hpp"

/**
 * @brief Implementation of the Vulkan renderer private methods
 *
 */
namespace engine
{

    void VulkanRenderer::pUpdateUniforms()
    {
        void *data;
        // auto allocation = getDeviceMemory(UNIFORM_BUFFER_PROP);
        vkMapMemory(m_pVkData.logicalDevice, m_pFrame.UniformBuffer().deviceMemory, 0, sizeof(ShaderData), 0, &data);
        memcpy(data, &m_pCameraData, sizeof(m_pCameraData));
        vkUnmapMemory(m_pVkData.logicalDevice, m_pFrame.UniformBuffer().deviceMemory);
    }

    void VulkanRenderer::pCreateVertexBuffer()
    {
        auto &buffer = m_pVertexBuffers.emplace_back();
        pCreateBuffer(buffer, sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, VERTEX_BUFFER_USAGE, VERTEX_BUFFER_PROP);
        // auto allocation = pGetOrCreateDeviceMemory(VERTEX_BUFFER_PROP, buffer);
        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(Vertex) * MAX_VERTICES_PER_BUFFER, " bytes in local vertex buffer");
    }

    void VulkanRenderer::pCreateIndexBuffer()
    {
        auto &buffer = m_pIndexBuffers.emplace_back();
        pCreateBuffer(buffer, sizeof(IndexType) * MAX_INDICES_PER_BUFFER, INDEX_BUFFER_USAGE, INDEX_BUFFER_PROP);

        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", sizeof(IndexType) * MAX_INDICES_PER_BUFFER, " bytes in local index buffer");
    }

    void VulkanRenderer::pCreateUniformBuffer(size_t size)
    {
        auto &buffer = m_pUniformBuffers.emplace_back();
        pCreateBuffer(buffer, size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP);
        // auto allocation = pGetOrCreateDeviceMemory(UNIFORM_BUFFER_PROP, buffer);
        // vkBindBufferMemory(m_pVkData.logicalDevice, buffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);

        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
    }

    void VulkanRenderer::pCreateStagingTextureBuffer(unsigned char *pixels, TextureInfo textureInfo)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        auto imageSize = textureInfo.width * textureInfo.height * textureInfo.numChannels;
        pCreateBuffer(m_pStagingTextureBuffer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void *data;
        vkMapMemory(m_pVkData.logicalDevice, m_pStagingTextureBuffer.deviceMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(m_pVkData.logicalDevice, m_pStagingTextureBuffer.deviceMemory);
    }

    vk::VulkanAllocation VulkanRenderer::pGetOrCreateDeviceMemory(VkMemoryPropertyFlags properties, const vk::VulkanBuffer &buffer)
    {

        vk::VulkanAllocation allocation;
        /*if (m_pMemoryAllocations.contains(properties))
        {
            allocation = m_pMemoryAllocations[properties];
        }
        else
        {
            allocation.deviceMemory = vk::allocateMemory(m_pVkData, buffer.buffer, properties);
            allocation.properties = properties;
            allocation.ownedBuffers.push_back(buffer);
        }*/

        allocation.allocatedBytes += size;

        return allocation;
    }

    vk::VulkanAllocation VulkanRenderer::getDeviceMemory(VkMemoryPropertyFlags properties)
    {
        vk::VulkanAllocation allocation;
        /* if (m_pMemoryAllocations.contains(properties))
         {
             allocation = m_pMemoryAllocations[properties];
         }*/
        return allocation;
    }

    void VulkanRenderer::pCreateBuffer(vk::VulkanBuffer &buffer, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        buffer.bufferInfo = vk::createVertexBuffer(m_pVkData, buffer.buffer, size, usage);
        buffer.deviceMemory = vk::allocateMemory(m_pVkData, buffer.buffer, properties);
        allocations_count++;
    }

    void VulkanRenderer::pCreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(ShaderData);

        // m_pUniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++)
        {
            pCreateUniformBuffer(bufferSize);
        }
    }

    vk::VulkanTexture VulkanRenderer::pCreateTextureBuffer(vk::VulkanTextureInfo texInfo)
    {
        auto &buffer = m_pTextureBuffers.emplace_back();
        auto texture = vk::createImage(m_pVkData, texInfo);
        buffer.deviceMemory = vk::allocateTextureMemory(m_pVkData, texture);
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", size, " bytes in local uniform buffer");
        return texture;
    }

    void VulkanRenderer::pCreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        poolInfo.maxSets = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT * 2);
        if (vkCreateDescriptorPool(m_pVkData.logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanRenderer::pCreateDescriptorSets(vk::VulkanMaterial& material)
    {
        std::vector<VkDescriptorSetLayout> layouts(vk::MAX_FRAMES_IN_FLIGHT, m_pRenderPasses.at(0).renderPipelines.back().descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_pDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();
        //material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        material.descriptorSets.resize(vk::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_pVkData.logicalDevice, &allocInfo, material.descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        pUpdateMaterial(material);

    }

    void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &material){

        for(int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_pUniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ShaderData);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = material.textures.at(0)->imageView;
            imageInfo.sampler = material.textures.at(0)->sampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = material.descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr;       // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = material.descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;
            
            vkUpdateDescriptorSets(m_pVkData.logicalDevice, 2, descriptorWrites.data(), 0, nullptr);
            //vk::updateDescriptorSet(m_pVkData, material);
        }
    }

    void VulkanRenderer::pCreateStagingBuffer(const std::vector<Vertex> &vertices)
    {
        // auto &buffer = m_pStagingBuffer.emplace_back();
        // pCreateBuffer(m_pStagingBuffer, vertices.size() * sizeof(Vertex), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP);
        m_pStagingBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingBuffer.buffer, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations_count++;

        // vk::VulkanAllocation allocation = pGetOrCreateDeviceMemory(STAGING_BUFFER_PROP, m_pStagingBuffer);

        // vkBindBufferMemory(m_pVkData.logicalDevice, m_pStagingBuffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        vk::mapMemory(m_pVkData, m_pStagingBuffer.deviceMemory, vertices.size() * sizeof(Vertex), m_pStagingBuffer.offset, vertices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", vertices.size() * sizeof(Vertex), " bytes in hosted staging buffer");
    }

    void VulkanRenderer::pCreateStagingIndexBuffer(const std::vector<IndexType> &indices)
    {
        m_pStagingIndexBuffer.bufferInfo = vk::createVertexBuffer(m_pVkData, m_pStagingIndexBuffer.buffer, indices.size() * sizeof(IndexType), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        m_pStagingIndexBuffer.deviceMemory = vk::allocateMemory(m_pVkData, m_pStagingIndexBuffer.buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        allocations_count++;
        // pCreateBuffer(m_pStagingIndexBuffer, indices.size() * sizeof(IndexType), STAGING_BUFFER_USAGE, STAGING_BUFFER_PROP);

        // vk::VulkanAllocation allocation = pGetOrCreateDeviceMemory(STAGING_BUFFER_PROP, m_pStagingIndexBuffer);

        // vkBindBufferMemory(m_pVkData.logicalDevice, m_pStagingBuffer.buffer, allocation.deviceMemory, allocation.allocatedBytes);
        vk::mapMemory(m_pVkData, m_pStagingIndexBuffer.deviceMemory, indices.size() * sizeof(IndexType), 0, indices.data());
        IO::Log("From function ", __PRETTY_FUNCTION__, " | Line ", __LINE__, " : ", "allocating ", indices.size() * sizeof(IndexType), " bytes in hosted staging buffer");
    }

    std::pair<vk::VulkanBuffer *, vk::VulkanBuffer *> VulkanRenderer::pGetBuffers(const VertexContainer &vertices, const IndexContainer &indices)
    {
        vk::VulkanBuffer *buffer;
        vk::VulkanBuffer *ibuffer;

        if (m_pVertexBuffers.size() < 1)
            &m_pVertexBuffers.emplace_back();
        if (m_pIndexBuffers.size() < 1)
            &m_pIndexBuffers.emplace_back();

        buffer = &m_pVertexBuffers.back();
        ibuffer = &m_pIndexBuffers.back();

        if (buffer->allocatedVertices + vertices.size() > MAX_VERTICES_PER_BUFFER ||
            ibuffer->allocatedVertices + indices.size() > MAX_INDICES_PER_BUFFER)
        {
            pCreateVertexBuffer();
            buffer = &m_pVertexBuffers.back();
            pCreateIndexBuffer();
            ibuffer = &m_pIndexBuffers.back();
        }

        return {buffer, ibuffer};
    }

}