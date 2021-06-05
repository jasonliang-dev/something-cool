#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define GLFW_INCLUDE_NONE
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4100)
    #pragma warning(disable : 4214)
    #pragma warning(disable : 4244)
    #pragma warning(disable : 4245)

    #include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cimgui/generator/output/cimgui_impl.h>
#include <cute_tiled.h>
#include <glad/gl.h>
#include <miniaudio.h>
#include <stb_image.h>
#include <stb_vorbis.c>

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#include "audio.h"
#include "input.h"
#include "language.h"
#include "memory.h"
#include "net.h"
#include "opengl_debug.h"
#include "os.h"
#include "player.h"
#include "renderer.h"
#include "sprite_animation.h"
#include "texture.h"
#include "tilemap.h"

typedef struct Application Application;
struct Application
{
    GLFWwindow *window;
    ImGuiContext *guiCTX;
    ImGuiIO *guiIO;
};

static void ErrorCallback(int code, const char *msg)
{
    fprintf(stderr, "GLFW Error %d: %s\n", code, msg);
}

static Application InitApplication(void)
{
    Application app = {0};

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

    app.window = glfwCreateWindow(1366, 768, "This is a title", NULL, NULL);
    if (!app.window)
    {
        Fatal("Cannot create window");
    }

    printf("Created window\n");

    glfwSetKeyCallback(app.window, InputKeyCallback);
    glfwMakeContextCurrent(app.window);
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

    app.guiCTX = igCreateContext(NULL);
    app.guiIO = igGetIO();
    app.guiIO->ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    igStyleColorsDark(NULL);

    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return app;
}

static void RunApplication(Application app)
{
    Texture atlas = CreateTexture("data/atlas.png");
    Tilemap map = CreateTilemap("data/test.json", atlas);
    Player player = CreatePlayer(v2(50, 50));
    SpriteAnimation ani = CreateSpriteAnimation(BIG_DEMON_IDLE_ANIM);
    InitClient();

    ScratchReset();

    f64 lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(app.window))
    {
        f64 now = glfwGetTime();
        f32 deltaTime = (f32)(now - lastTime);
        lastTime = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        //

        if (KeyPressed(GLFW_KEY_F4))
        {
            ClientSend("packet");
        }

        UpdatePlayer(&player, &map, deltaTime);
        UpdateSpriteAnimation(&ani, deltaTime);

        //

        static bool s_Demo = false;
        static char s_ServerPort[16] = "4242";
        static char s_RemoteHost[256] = "127.0.0.1";
        static char s_RemotePort[16] = "4242";

        igBegin("Debug Window", NULL, 0);

        igInputText("Server Port", s_ServerPort, ArrayCount(s_ServerPort),
                    ImGuiInputTextFlags_CharsDecimal, 0, 0);
        if (igButton("Create server host", (ImVec2){0, 0}))
        {
            InitServer((u16)atoi(s_ServerPort));
        }

        igInputText("Remote Host", s_RemoteHost, ArrayCount(s_RemoteHost), 0, 0, 0);
        igInputText("Port", s_RemotePort, ArrayCount(s_RemotePort),
                    ImGuiInputTextFlags_CharsDecimal, 0, 0);
        if (igButton("Connect to host", (ImVec2){0, 0}))
        {
            NetError err = ClientConnect(s_RemoteHost, (u16)atoi(s_RemotePort));
            if (err)
            {
                fprintf(stderr, "error: %s\n", err);
            }
        }

        igCheckbox("Demo Window", &s_Demo);
        igText("Application average %.4f ms/frame (%.1f FPS)", deltaTime, 1 / deltaTime);
        igEnd();

        if (s_Demo)
        {
            igShowDemoWindow(&s_Demo);
        }

        //

        i32 windowWidth;
        i32 windowHeight;
        glfwGetFramebufferSize(app.window, &windowWidth, &windowHeight);
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
        DrawSpriteAnimation(&ani, v2(100, 100));
        DrawPlayer(&player);
        EndDraw();

        igRender();
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        glfwSwapBuffers(app.window);

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
    Application app = InitApplication();
    InitRenderer();

    RunApplication(app);
}
