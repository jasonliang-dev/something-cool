#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

#ifdef _MSC_VER
#include <SDL.h> // windows
#else
#include <SDL2/SDL.h> // linux
#endif

#include "GL/gl3w.h"

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "language.h"
#include "app.h"

global const i32 SCREEN_WIDTH = 1366;
global const i32 SCREEN_HEIGHT = 768;

global const i32 DRAW_SCALE = 4;

global const char *WINDOW_TITLE = "This is a title";

global AppState *app = NULL;

#include "gl3w.c"

#include "imgui.cpp"
#include "imgui_impl_sdl.cpp"
#include "imgui_impl_opengl3.cpp"
#include "imgui_draw.cpp"
#include "imgui_tables.cpp"
#include "imgui_widgets.cpp"

// optional
#include "imgui_demo.cpp"

#include "maths.cpp"
// #include "render.cpp"
// #include "tilemap.cpp"
// #include "entity.cpp"

internal inline bool KeyPress(SDL_Scancode scancode)
{
    return !app->keyDownPrev[scancode] && app->keyDown[scancode];
}

internal inline bool KeyRelease(SDL_Scancode scancode)
{
    return app->keyDownPrev[scancode] && !app->keyDown[scancode];
}

i32 main(i32 argc, char *argv[])
{
    (void)argc;
    (void)argv;

    {
        // init app
        app = (AppState *)malloc(sizeof(AppState));
        assert(app);
        app->debug = false;
        app->running = true;

        // init sdl
        assert(SDL_Init(SDL_INIT_VIDEO) == 0);

#ifdef __APPLE__
        // GL 3.2 Core + GLSL 150
        const char *glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

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
        SDL_GL_SetSwapInterval(1); // enable vsync

        // load opengl procs
        assert(gl3wInit() == 0);

        // init imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        app->imguiIO = &ImGui::GetIO();
        app->imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // style
        ImGui::StyleColorsDark();

        // backend
        ImGui_ImplSDL2_InitForOpenGL(app->window, app->glContext);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // keyboard state
        app->keyDown = SDL_GetKeyboardState(&app->keyCount);
        app->keyDownPrev = (u8 *)malloc(app->keyCount);
        assert(app->keyDownPrev);
    }

    bool show_demo_window = true;
    bool show_another_window = false;

    v4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

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
        // (void *) discarding const
        memcpy((void *)app->keyDownPrev, app->keyDown, app->keyCount);

        while (SDL_PollEvent(&event) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
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

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(app->window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You
        // can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a
        // named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin(
                "Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format
                                                      // strings too)
            ImGui::Checkbox("Demo Window",
                            &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f,
                               1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color",
                              (float *)&clear_color); // Edit 3 floats representing a color

            // Buttons return true when clicked (most widgets return
            // true when edited/activated)
            if (ImGui::Button("Button"))
            {
                counter++;
            }

            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            // Pass a pointer to our bool variable (the window will have
            // a closing button that will clear the bool when clicked)
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
            {
                show_another_window = false;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)app->imguiIO->DisplaySize.x, (int)app->imguiIO->DisplaySize.y);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

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

        free(app);
    }

    return 0;
}
