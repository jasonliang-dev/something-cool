#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "app.hpp"
#include "input.hpp"
#include "language.hpp"
#include "opengl_debug.hpp"
#include "renderer.hpp"
#include "sprite_animation.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
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
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

static void RunApplication(void)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Cannot initialize glfw");
    }

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

    // glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, Input_KeyCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

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

    std::shared_ptr<Texture> atlas = std::make_shared<Texture>("data/atlas.png");
    Tilemap map{"data/test.json", atlas};

    std::ifstream atlasListFile("data/atlas_list.txt");

    std::unordered_map<std::string, v4> crops;
    std::unordered_map<std::string, SpriteAnimation> animations;

    std::string line;
    while (std::getline(atlasListFile, line))
    {
        std::istringstream ss(line);
        std::string name;
        ss >> name;

        if (name == "")
        {
            continue;
        }

        v4 rect;
        if (!(ss >> rect.x >> rect.y >> rect.z >> rect.w))
        {
            throw std::runtime_error("Malformed atlas data");
        }

        i32 count;
        if (ss >> count)
        {
            animations[name] = {rect, count, 100};
        }
        else
        {
            crops[name] = rect;
        }
    }

    Renderer renderer;

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool showDemoWindow = false;
    bool overlay = true;

    f32 scale = 3.0f;

    const char *animationList[] = {
        "tiny_zombie_idle_anim", "tiny_zombie_run_anim",  "goblin_idle_anim",
        "goblin_run_anim",       "imp_idle_anim",         "imp_run_anim",
        "skelet_idle_anim",      "skelet_run_anim",       "muddy_idle_anim",
        "muddy_run_anim",        "swampy_idle_anim",      "swampy_run_anim",
        "zombie_idle_anim",      "zombie_run_anim",       "ice_zombie_idle_anim",
        "ice_zombie_run_anim",   "masked_orc_idle_anim",  "masked_orc_run_anim",
        "orc_warrior_idle_anim", "orc_warrior_run_anim",  "orc_shaman_idle_anim",
        "orc_shaman_run_anim",   "necromancer_idle_anim", "necromancer_run_anim",
        "wogol_idle_anim",       "wogol_run_anim",        "chort_idle_anim",
        "chort_run_anim",        "big_zombie_idle_anim",  "big_zombie_run_anim",
        "ogre_idle_anim",        "ogre_run_anim",         "big_demon_idle_anim",
        "big_demon_run_anim",    "elf_f_idle_anim",       "elf_f_run_anim",
        "elf_f_hit_anim",        "elf_m_idle_anim",       "elf_m_run_anim",
        "elf_m_hit_anim",        "knight_f_idle_anim",    "knight_f_run_anim",
        "knight_f_hit_anim",     "knight_m_idle_anim",    "knight_m_run_anim",
        "knight_m_hit_anim",     "wizzard_f_idle_anim",   "wizzard_f_run_anim",
        "wizzard_f_hit_anim",    "wizzard_m_idle_anim",   "wizzard_m_run_anim",
        "wizzard_m_hit_anim",    "lizard_f_idle_anim",    "lizard_f_run_anim",
        "lizard_f_hit_anim",     "lizard_m_idle_anim",    "lizard_m_run_anim",
        "lizard_m_hit_anim",
    };

    int currentAnimation = 0;

    f64 lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        f64 now = glfwGetTime();
        f32 deltaTime = (f32)(now - lastTime);
        lastTime = now;

        if (KeyPressed(GLFW_KEY_F3))
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
                ImGui::ListBox("animation", &currentAnimation, animationList,
                               ArrayCount(animationList), 10);
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

        // map.Draw(renderer, projection * model);

        renderer.BeginDraw(atlas, projection * model);
        SpriteAnimation &ani = animations[animationList[currentAnimation]];
        ani.Update(deltaTime);
        renderer.DrawTexture(v2(10.0f, 10.0f), ani.GetFrameRect());
        renderer.EndDraw();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        g_Input.Update();
        glfwPollEvents();
    }

    glfwTerminate();
}

int main(void)
{
    try
    {
        RunApplication();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        abort();
    }
}
