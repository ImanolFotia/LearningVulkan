#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <set>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "framework/common.hpp"
#include "framework/def.hpp"
#include "framework/window.hpp"

#include "engine/renderers/vulkan.hpp"



namespace LearningVulkan
{
    class LearningVulkanApplication
    {

        uint32_t m_CurrentFrame = 0;
        framework::Window m_Window;
        std::string m_ApplicationName = "Default";

        std::unique_ptr<engine::Renderer> m_pRenderer;

    public:
        LearningVulkanApplication() = default;

        LearningVulkanApplication(std::string appName) : m_ApplicationName(appName) {
            m_pRenderer = std::make_unique<engine::VulkanRenderer>();
        }

        void run()
        {
            initWindow();
            initVulkan();
            onCreate();
            mainLoop();
            exit();
        }

        virtual void onCreate() = 0;
        virtual void onRender() = 0;
        virtual void onExit() = 0;

    protected:


        bool mShouldClose = false;
        void ShouldClose()
        {
            mShouldClose = true;
        }


    private:
        void initWindow()
        {
            m_Window.init(m_ApplicationName, 1280, 720);
        }

        void initVulkan()
        {
            m_pRenderer->Init(m_ApplicationName.c_str(), m_Window);
        }

        void mainLoop()
        {
            while (!m_Window.ShouldClose())
            {
                if (mShouldClose)
                    break;
                onRender();
                drawFrame();
                m_Window.PollEvents();
            }

        }

        void drawFrame()
        {
            m_pRenderer->Begin();

            m_pRenderer->Flush();
            
            m_pRenderer->End();
        }

        void exit()
        {
            onExit();
            m_pRenderer->Cleanup();
            m_Window.cleanup();
        }

        protected:
            void Submit(const std::vector<engine::Vertex>& vertices, const engine::MaterialInfo& materialInfo) {
                m_pRenderer->Stage(vertices, materialInfo);
            }
    };
}