#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include "app.hpp"
#include "input.hpp"
#include "language.hpp"
#include "opengl_debug.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cute_tiled.h>
#include <fstream>
#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stb_image.h>
#include <stdexcept>
#include <thread>
#include <vector>

AppState app;

static void RunApplication(void)
{
#ifdef __APPLE__
    // Required on Mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(1366, 768, "This is a title", NULL, NULL);
    if (!window)
    {
        throw std::runtime_error("Cannot create window");
    }

    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, Input_KeyCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        throw std::runtime_error("Cannot load OpenGL procs");
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

#ifndef NDEBUG
    i32 flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) && glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GL_DebugMessageCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                              0, NULL, GL_FALSE);
    }
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    Tilemap map{"data/test.json", std::make_shared<Texture>("data/atlas.png")};

    Renderer renderer;

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool showDemoWindow = false;
    bool overlay = true;

    f32 scale = 3.0f;

    while (!glfwWindowShouldClose(window))
    {
        if (app.input.KeyPressed(GLFW_KEY_F3))
        {
            overlay = !overlay;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (overlay)
        {
            if (showDemoWindow)
            {
                ImGui::ShowDemoWindow(&showDemoWindow);
            }

            ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav;

            ImGui::SetNextWindowBgAlpha(0.80f);
            if (ImGui::Begin("Hello, world!", &overlay, windowFlags))
            {
                ImGui::Checkbox("Demo Window", &showDemoWindow);
                ImGui::DragFloat("scale", &scale, 0.01f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            ImGui::End();
        }

        ImGui::Render();

        i32 windowWidth;
        i32 windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m4 projection =
            glm::ortho(0.0f, (f32)windowWidth, (f32)windowHeight, 0.0f, -1.0f, 1.0f);
        m4 model = glm::scale(m4(1.0f), v3(scale, scale, 1.0f));

        map.Draw(renderer, projection * model);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        app.input.Update();
        glfwPollEvents();
    }
}

int main(void)
{
    try
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Cannot initialize glfw");
        }

        RunApplication();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        abort();
    }

    glfwTerminate();
}
