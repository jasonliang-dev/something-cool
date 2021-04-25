#include "camera2d.h"
#include "camera3d.h"
#include "geometry.h"
#include "gfx.h"
#include "gl.h"
#include "input.h"
#include "shader.h"
#include "shaders.gen.h"
#include "texture.h"
#include "window.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    if (!WindowCreate(1366, 768, "This is a title"))
    {
        fprintf(stderr, "Failed to create a window\n");
        abort();
    }

    Camera3D camera;
    Camera3DCreate(&camera, v3(0.0f, 0.0f, -3.0f), PI / 2.0f, 0.0f);

    Texture tex = TextureCreate("data/Blue.bmp");

    Shader basicShader = ShaderCreate(BASIC_VERT, BASIC_FRAG);
    ShaderSetInt(basicShader, "u_Texture", 0);

    f32 vertices[] = {
        // xyz            uv
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //
    };

    VAO vao = VAOCreate();
    VBO vbo = VBOCreate(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    VertexAttrBegin(sizeof(f32) * 5);
    VertexAttrPush(3);
    VertexAttrPush(2);

    InputInit();
    WindowSwapInterval(1);
    while (!WindowShouldClose())
    {
        v2 look = v2(0, 0);
        f32 sensitivity = 6.0f;
        if (KeyDown(Key_Up))
            look.y += sensitivity;
        if (KeyDown(Key_Down))
            look.y -= sensitivity;
        if (KeyDown(Key_Left))
            look.x -= sensitivity;
        if (KeyDown(Key_Right))
            look.x += sensitivity;

        Camera3DUpdateLook(&camera, look);

        f32 moveSpeed = 0.2f;
        if (KeyDown(Key_W))
            camera.position = V3PlusV3(camera.position, V3xF32(camera.forward, moveSpeed));
        if (KeyDown(Key_S))
            camera.position = V3PlusV3(camera.position, V3xF32(camera.forward, -moveSpeed));
        if (KeyDown(Key_A))
            camera.position = V3PlusV3(camera.position, V3xF32(camera.right, -moveSpeed));
        if (KeyDown(Key_D))
            camera.position = V3PlusV3(camera.position, V3xF32(camera.right, moveSpeed));

        if (KeyDown(Key_Space))
            camera.position.y -= moveSpeed;
        if (KeyDown(Key_LeftShift))
            camera.position.y += moveSpeed;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        Camera3DUpdateViewProj(&camera, WindowV2());

        ShaderUse(basicShader);
        ShaderSetM4(basicShader, "u_View", camera.view);
        ShaderSetM4(basicShader, "u_Projection", camera.projection);

        m4 model = M4Scale(M4Identity(), v3(4, 4, 4));
        ShaderSetM4(basicShader, "u_Model", model);

        TextureBind(tex, 0);

        VAOBind(vao);
        VBOBind(vbo);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        WindowSwapBuffers();
        InputUpdate();
        WindowPollEvents();
    }
}
