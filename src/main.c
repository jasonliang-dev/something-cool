#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "input.h"
#include "language.h"
#include "memory.h"
#include "opengl_debug.h"
#include "os.h"
#include "player.h"
#include "renderer.h"
#include "sprite_animation.h"
#include "texture.h"
#include "tilemap.h"
#include <GLFW/glfw3.h>
#include <cute_tiled.h>
#include <glad/gl.h>
#include <stb_image.h>

GLFWwindow *g_Window;

static void InitWindow(void)
{
    if (!glfwInit())
    {
        Fatal("Cannot initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // Required on Mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    g_Window = glfwCreateWindow(1366, 768, "This is a title", NULL, NULL);
    if (!g_Window)
    {
        Fatal("Cannot create window");
    }

    printf("Created window\n");

    glfwSetKeyCallback(g_Window, InputKeyCallback);
    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(1);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        Fatal("Cannot load OpenGL procs");
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

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

        printf("Enabled OpenGL debug output\n");
    }
#endif
}

static void RunApplication(void)
{
    Texture atlas = CreateTexture("data/atlas.png");
    Tilemap map = CreateTilemap("data/test.json", atlas);
    Player player = CreatePlayer();

    ScratchReset();

    f64 lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(g_Window))
    {
        f64 now = glfwGetTime();
        f32 deltaTime = (f32)(now - lastTime);
        lastTime = now;

        UpdatePlayer(&player, deltaTime);

        //

        i32 windowWidth;
        i32 windowHeight;
        glfwGetFramebufferSize(g_Window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m4 projection =
            M4Orthographic(0.0f, (f32)windowWidth, (f32)windowHeight, 0.0f, -1.0f, 1.0f);
        m4 view = M4Scale(m4(1), v3(3, 3, 1));

        BeginDraw(atlas, M4xM4(projection, view));
        DrawTilemap(&map);
        DrawPlayer(&player);
        EndDraw();

        glfwSwapBuffers(g_Window);

        UpdateInput();
        glfwPollEvents();
    }
}

int main(void)
{
    printf("OS page size: %d\n", GetSystemPageSize());

    InitMemoryArenas();
    InitInput();
    InitWindow();
    InitRenderer();

    RunApplication();
}
