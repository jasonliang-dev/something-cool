#include "camera2d.h"
#include "geometry.h"
#include "gfx.h"
#include "gl.h"
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

    WindowSwapInterval(1);

    Camera2D camera = Camera2DCreate();
    Texture tex = TextureCreate("data/test.bmp");

    Shader basicShader = ShaderCreate(BASIC_VERT, BASIC_FRAG);
    ShaderSetInt(basicShader, "u_Texture", 0);

    f32 vertices[] = {
        // xyz            tex
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //

        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f  //
    };

    VAO vao = VAOCreate();
    VBO vbo = VBOCreate(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    VertexAttrBegin(sizeof(f32) * 5);
    VertexAttrPush(3);
    VertexAttrPush(2);

    while (!WindowShouldClose())
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        Camera2DUpdate(&camera, WindowV2());

        ShaderUse(basicShader);
        ShaderSetM4(basicShader, "u_View", M4Identity());
        ShaderSetM4(basicShader, "u_Projection", camera.projection);

        m4 model = M4Translate(M4Identity(), v3(50, 50, 0));
        model = M4Scale(model, v3(tex.width * 32.0f, tex.height * 32.0f, 1));
        ShaderSetM4(basicShader, "u_Model", model);

        TextureBind(tex, 0);

        VAOBind(vao);
        VBOBind(vbo);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        WindowSwapBuffers();
        WindowPollEvents();
    }
}
