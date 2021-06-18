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

// complaints about macro redefinitions because of glfw (maybe enet? getting ws2 errors)
// include windows.h beforehand to fix this
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define CUTE_TILED_IMPLEMENTATION
#include <cute_tiled.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
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

#include <time.h>

#include "assets.h"
#include "audio.h"
#include "input.h"
#include "memory.h"
#include "net.h"
#include "renderer.h"
#include "scenes.h"
#include "ui.h"
#include "window.h"

int main(void)
{
    srand((u32)time(NULL));

    InitMemoryArenas();
    InitInput();
    InitAudio();
    InitNet();
    InitWindow(1366, 768);
    glfwSwapInterval(1); // vsync
    InitRenderer();
    InitPermanentAssets();
    InitUI();
    InitScenes(SCENE_MAIN_MENU);
    // InitScenes(SCENE_CUSTOMIZE);

    f64 lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(g_Window.handle))
    {
        f64 now = glfwGetTime();
        f32 deltaTime = (f32)(now - lastTime);
        lastTime = now;

        ScratchReset();

        UpdateScene(deltaTime);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetFramebufferSize(g_Window.handle, &g_Window.width, &g_Window.height);
        glViewport(0, 0, g_Window.width, g_Window.height);

        m4 projection = M4Orthographic(0.0f, (f32)g_Window.width, (f32)g_Window.height,
                                       0.0f, -1.0f, 1.0f);

        DrawScene(projection);

        glfwSwapBuffers(g_Window.handle);

        SceneFinishFrame();

        UpdateInput();
        glfwPollEvents();

        ServerPollEvents();
        ClientPollEvents();
    }
}
