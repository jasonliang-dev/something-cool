#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct Application
{
    GLFWwindow *m_Window;

    Application(void);
    ~Application(void);

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void Run(void);
};
