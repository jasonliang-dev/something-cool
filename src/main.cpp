#include "language.hpp"
#include "opengl_debug.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <algorithm>
#include <enet/enet.h>
#include <fstream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <iostream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <thread>
#include <vector>

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

    Texture tex{"data/Blue.png"};

    auto ReadFile = [](const char *filePath)
    {
        std::ifstream file(filePath);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file");
        }

        std::string data{std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>()};

        file.close();
        return data;
    };

    Shader shader{ReadFile("data/basic.vert").c_str(),
                  ReadFile("data/basic.frag").c_str()};
    shader.Use();
    shader.SetInt("u_Texture", 0);

    f32 vertices[] = {
        // xy             uv
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f  //
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(0));

    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool showDemoWindow = false;

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        static bool s_Overlay = true;
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav;

        ImGui::SetNextWindowBgAlpha(0.80f);
        if (ImGui::Begin("Hello, world!", &s_Overlay, windowFlags))
        {
            ImGui::Checkbox("Demo Window", &showDemoWindow);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::End();

        ImGui::Render();

        i32 windowWidth;
        i32 windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        tex.Bind(0);

        shader.Use();
        shader.SetM4("u_View", m4(1));

        shader.SetM4("u_Projection", glm::ortho(0.0f, (f32)windowWidth, (f32)windowHeight,
                                                0.0f, -1.0f, 1.0f));

        m4 model = glm::translate(m4(1), v3(50.0f, 50.0f, 0.0f));
        model = glm::scale(model, v3(64.0f, 64.0f, 1.0f));

        shader.SetM4("u_Model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
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

        if (enet_initialize() != 0)
        {
            throw std::runtime_error("Cannot initialize ENet");
        }

        RunApplication();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        abort();
    }

    enet_deinitialize();
    glfwTerminate();
}
