//
// Created by solaire on 2/2/23.
//

#include "window.hpp"

#include "core/framework/IO/KeyBoard.hpp"
#include "core/framework/IO/Mouse.hpp"
#include "core/framework/IO/Joystick/Joystick.hpp"

namespace framework
{
    void Window::init(std::string appName, int w, int h)
    {
        mWidth = w;
        mHeight = h;

#if USE_GLFW
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        mWindow = glfwCreateWindow(mWidth, mHeight, appName.c_str(), nullptr, nullptr);

        // Set up IO callbacks
        glfwSetKeyCallback(mWindow, Input::KeyBoard::KeyBoardCallBackGLFW);
        glfwSetCursorPosCallback(mWindow, Input::Mouse::MouseCallBackGLFW);
        glfwSetJoystickCallback(Input::Joystick::JoystickManager::JoystickCallbackGLFW);
        glfwSetMouseButtonCallback(mWindow, Input::Mouse::MouseButtonCallbackGLFW);
        glfwSetScrollCallback(mWindow, Input::Mouse::MouseWheelCallbackGLFW);

        glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, true);
        glfwSetInputMode(mWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
#endif
#if defined(_WIN32) && (USE_GLFW == false)
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = appName;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
        if (!RegisterClassEx(&wcex))
            exitOnError("Failed to register window");

        windowInstance = hInstance;

        window = CreateWindow(
            appName,
            appName,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            0,
            0,
            mWidth,
            mWidth,
            NULL,
            NULL,
            windowInstance,
            NULL);
#endif
#ifdef BUILD_ANDROID
#endif
    }
}