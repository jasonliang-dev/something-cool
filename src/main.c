#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#pragma warning(push)
#pragma warning(disable : 4214)

#include <windows.h>

#include <GLFW/glfw3.h>
#include <cimgui.h>
#include <cute_tiled.h>
#include <enet/enet.h>
#include <generator/output/cimgui_impl.h>
#include <glad/gl.h>
#include <stb_image.h>

#pragma warning(pop)

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
        glDebugMessageCallback(GL_DebugMessageCallback, 0);
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

    ENetHost *server = NULL;
    ENetHost *client = NULL;
    ENetPeer *peer = NULL;

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

        if (server)
        {
            ENetEvent event;
            while (enet_host_service(server, &event, 0) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",
                           event.peer->address.host, event.peer->address.port);
                    event.peer->data = "Client information";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("A packet of length %u containing %s was received from %s on "
                           "channel %u.\n",
                           event.packet->dataLength, event.packet->data, event.peer->data,
                           event.channelID);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
                    event.peer->data = NULL;
                }
            }
        }

        if (client)
        {
            ENetEvent event;
            while (enet_host_service(client, &event, 0) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("connected to host %x:%u.\n",
                           event.peer->address.host, event.peer->address.port);
                    event.peer->data = "Host information";
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
                    event.peer->data = NULL;
                }
            }
        }

        //

        if (!server && KeyPressed(GLFW_KEY_F2))
        {
            ENetAddress address;
            address.host = ENET_HOST_ANY;
            address.port = 4242;

            // 32 connections, 2 channels, 0 and 1
            server = enet_host_create(&address, 32, 2, 0, 0);

            if (!server)
            {
                Fatal("Can't create server host");
            }
            else
            {
                printf("Created server\n");
            }
        }

        if (!client && KeyPressed(GLFW_KEY_F3))
        {
            client = enet_host_create(NULL, 1, 2, 0, 0);

            if (!client)
            {
                Fatal("Can't create client host");
            }
            else
            {
                printf("Created client\n");
            }

            ENetAddress address;
            assert(enet_address_set_host(&address, "127.0.0.1") == 0);
            address.port = 4242;

            peer = enet_host_connect(client, &address, 2, 0);

            if (!peer)
            {
                Fatal("No peer available\n");
            }
        }

        if (client && KeyPressed(GLFW_KEY_F4))
        {
            ENetPacket *packet = enet_packet_create("packet", strlen("packet") + 1,
                                                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            enet_host_flush(client);
        }

        UpdatePlayer(&player, &map, deltaTime);
        UpdateSpriteAnimation(&ani, deltaTime);

        //

        static bool s_Demo = false;

        igBegin("Test", NULL, 0);
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
    }
}

int main(void)
{
    printf("OS page size: %d\n", GetSystemPageSize());

    InitMemoryArenas();
    InitInput();
    Application app = InitApplication();
    InitRenderer();

    if (enet_initialize() != 0)
    {
        Fatal("Cannot initialize ENet");
    }

    RunApplication(app);
}
