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

#include <GL/gl3w.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <gl3w.c>

#include <imgui.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui_widgets.cpp>
#include <imgui_demo.cpp>

#define IMGUI_IMPL_OPENGL_LOADER_GL3W
#include <imgui_impl_opengl3.cpp>
#include <imgui_impl_sdl.cpp>

#include "language.h"
#include "render.h"
#include "shaders.gen.h"
#include "app.h"

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

// global const i32 DRAW_SCALE = 4;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = nullptr;

#include "maths.cpp"
#include "render.cpp"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    {
        // init app
        app = (AppState *)malloc(sizeof(AppState));
        assert(app);
        app->debug = false;
        app->running = true;
        app->showImguiDemoWindow = false;

        // init sdl
        assert(SDL_Init(SDL_INIT_VIDEO) == 0);

#ifdef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        app->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
                                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
        assert(app->window);
        app->glContext = SDL_GL_CreateContext(app->window);
        SDL_GL_MakeCurrent(app->window, app->glContext);
        SDL_GL_SetSwapInterval(0); // vsync

        // load opengl procs
        assert(gl3wInit() == 0);

        SetupRenderer(&app->renderer);
        app->dog = CreateTexture("data/dog.png");

        // init imgui
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            // style
            ImGui::StyleColorsDark();

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
        memcpy(const_cast<u8 *>(app->keyDownPrev), app->keyDown, app->keyCount);

        while (SDL_PollEvent(&event) != 0)
        {
            // ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                app->running = false;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                     event.window.windowID == SDL_GetWindowID(app->window))
            {
                app->running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(app->window);
        ImGui::NewFrame();

        if (app->showImguiDemoWindow)
        {
            ImGui::ShowDemoWindow(&app->showImguiDemoWindow);
        }

        {
            ImGui::Begin("Hello, world!");

            ImGui::Checkbox("Demo Window", &app->showImguiDemoWindow);

            ImGui::SliderFloat("dog x", &dogPosition.x, 0.0f, SCREEN_WIDTH);
            ImGui::SliderFloat("dog y", &dogPosition.y, 0.0f, SCREEN_HEIGHT);

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

        DrawSpriteExt(app->dog, dogPosition, 0, v2(4, 4), v2(0, 0));

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(app->window);
    }

    // free all resources
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(app->glContext);
        SDL_DestroyWindow(app->window);
        SDL_Quit();

        free(const_cast<u8 *>(app->keyDownPrev));
        free(app);
    }

    return 0;
}
