#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <SDL.h> // windows
#include <SDL_syswm.h>
#else
#include <SDL2/SDL.h> // linux
#include <SDL2/SDL_syswm.h>
#endif

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad.c>

#include <imgui.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui_widgets.cpp>
#include <imgui_demo.cpp>

#include <imgui_impl_opengl3.cpp>
#include <imgui_impl_sdl.cpp>

#include "language.h"
#include "render.h"
#include "shaders.gen.h"
#include "app.h"

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = nullptr;

#include "utils.cpp"
#include "maths.cpp"
#include "gl.cpp"
#include "render.cpp"
#include "imgui_style.cpp"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    {
        // init app
        app = (AppState *)malloc(sizeof(AppState));
        assert(app);
        app->running = true;
        app->showImguiDemoWindow = false;

        // init sdl
        assert(SDL_Init(SDL_INIT_VIDEO) == 0);

        i32 glContextFlags = 0;
#ifdef __APPLE__
        // Always required on Mac
        glContextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#endif

#ifdef DEBUG
        glContextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, glContextFlags);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        app->windowWidth = SCREEN_WIDTH;
        app->windowHeight = SCREEN_HEIGHT;
        app->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
                                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
        assert(app->window);
        app->glContext = SDL_GL_CreateContext(app->window);
        SDL_GL_MakeCurrent(app->window, app->glContext);
        SDL_GL_SetSwapInterval(0); // vsync

        // load opengl procs
        assert(gladLoadGLLoader(SDL_GL_GetProcAddress));
        printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        if (glDebugMessageCallback)
        {
            glDebugMessageCallback(GL_MessageCallback, 0);
        }

        if (glDebugMessageControl)
        {
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0,
                                  NULL, GL_FALSE);
        }
#endif

        SetupRenderer(&app->renderer);
        app->dog = CreateTexture("data/dog.png");

        // init imgui
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            // style
            ImGuiCustomDark();

            // backend
            ImGui_ImplSDL2_InitForOpenGL(app->window, app->glContext);
            ImGui_ImplOpenGL3_Init("#version 330 core");
        }

        // keyboard state
        app->keyDown = SDL_GetKeyboardState(&app->keyCount);
        app->keyDownPrev = (const u8 *)malloc(app->keyCount);
        assert(app->keyDownPrev);
    }

    v2 dogPosition = v2(100, 100);
    f32 rotation = 0.0f;
    v4 clearColor = v4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Event event;

    u64 counterCurrent = SDL_GetPerformanceCounter();
    u64 counterNew;
    while (app->running)
    {
        counterNew = SDL_GetPerformanceCounter();
        app->deltaTime = (counterNew - counterCurrent) / (f32)SDL_GetPerformanceFrequency();
        counterCurrent = counterNew;

        // app->keyDown updates after event loop.
        // must update keyDownPrev before the loop.
        memcpy((void *)app->keyDownPrev, app->keyDown, app->keyCount);

        while (SDL_PollEvent(&event) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                app->running = false;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
                    event.window.windowID == SDL_GetWindowID(app->window))
                {
                    app->running = false;
                }
                else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    app->windowWidth = event.window.data1;
                    app->windowHeight = event.window.data2;
                    UpdateProjections(&app->renderer);
                }
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(app->window);
        ImGui::NewFrame();

        if (app->showImguiDemoWindow)
        {
            ImGui::ShowDemoWindow((bool *)&app->showImguiDemoWindow);
        }

        {
            ImGui::Begin("Hello, world!");

            ImGui::Checkbox("Demo Window", (bool *)&app->showImguiDemoWindow);

            ImGui::SliderFloat("dog x", &dogPosition.x, 0.0f, (f32)app->windowWidth);
            ImGui::SliderFloat("dog y", &dogPosition.y, 0.0f, (f32)app->windowHeight);

            ImGui::SliderFloat("dog rot", &rotation, 0.0f, PI * 2);

            ImGui::ColorEdit3("clear color", (float *)&clearColor);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGuiIO &io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);

        DrawTexture(app->dog, dogPosition, rotation, v2(4, 4), v2(0, 0));

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(app->window);

        GL_CheckForErrors();
    }

    // free all resources
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        DestroyRenderer(&app->renderer);

        SDL_GL_DeleteContext(app->glContext);
        SDL_DestroyWindow(app->window);
        SDL_Quit();

        free((void *)app->keyDownPrev);
        free(app);
    }

    return 0;
}
