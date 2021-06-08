#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4100) // unreferenced formal parameter
    #pragma warning(disable : 4244) // narrowing conversion, possible loss of data
    #pragma warning(disable : 4245) // signed/unsigned mismatch
    #pragma warning(disable : 4996) // deprecated api
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// complaints about macro redefinitions
// include windows.h to fix this
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui/generator/output/cimgui_impl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#undef STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include "audio.h"
#include "input.h"
#include "memory.h"
#include "net.h"
#include "opengl_debug.h"
#include "os.h"
#include "renderer.h"
#include "scenes.h"

static void ErrorCallback(int code, const char *msg)
{
    fprintf(stderr, "GLFW Error %d: %s\n", code, msg);
}

static GLFWwindow *InitWindow(void)
{
    glfwSetErrorCallback(ErrorCallback);
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

    GLFWwindow *window = glfwCreateWindow(1366, 768, "This is a title", NULL, NULL);
    if (!window)
    {
        Fatal("Cannot create window");
    }

    printf("Created window\n");

    glfwSetKeyCallback(window, InputKeyCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

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
        glDebugMessageCallback(GLDebugMessageCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                              0, NULL, GL_FALSE);

        printf("Enabled OpenGL debug output\n");
    }
#endif

    igCreateContext(NULL);
    ImGuiIO *guiIO = igGetIO();
    guiIO->ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    igStyleColorsDark(NULL);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return window;
}

static void RunApplication(GLFWwindow *window)
{
    f64 lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        f64 now = glfwGetTime();
        f32 deltaTime = (f32)(now - lastTime);
        lastTime = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        UpdateScene(deltaTime);

        i32 windowWidth;
        i32 windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m4 projection =
            M4Orthographic(0.0f, (f32)windowWidth, (f32)windowHeight, 0.0f, -1.0f, 1.0f);

        DrawScene(projection);

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        glfwSwapBuffers(window);

        SceneFinishFrame();

        UpdateInput();
        glfwPollEvents();

        ServerPollEvents();
        ClientPollEvents();
    }
}

int main(void)
{
    printf("OS page size: %d\n", GetSystemPageSize());

    InitMemoryArenas();
    InitInput();
    InitAudio();
    InitNet();
    GLFWwindow *window = InitWindow();
    InitRenderer();
    InitScenes(SCENE_MAIN_MENU);

    ScratchReset();

    RunApplication(window);
}
