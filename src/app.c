#include "app.h"

char *vertexSource = "\
    #version 330 core                          \n\
    layout(location = 0) in vec2 position;     \n\
    void main()                                \n\
    {                                          \n\
        gl_Position = vec4(position, 0.0, 1.0);\n\
    }                                          \n\
";

char *fragmentSource = "\
    #version 330 core                    \n\
    layout(location = 0) out vec4 color; \n\
    void main()                          \n\
    {                                    \n\
        color = vec4(1.0, 0.0, 0.0, 1.0);\n\
    }                                    \n\
";

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("APP_PERMANENT_LOAD\n");
    LoadOpenGLProcedures();

    f32 positions[] = {
        -0.5f, -0.5f, //
        0.0f,  0.5f,  //
        0.5f,  -0.5f  //
    };

    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(f32) * 2, 0);

    u32 shader = R_CreateShader(vertexSource, fragmentSource);
    glUseProgram(shader);
}

APP_HOT_LOAD
{
    os = os_;
    os->DebugPrint("APP_HOT_LOAD\n");
}

APP_HOT_UNLOAD
{
    os->DebugPrint("APP_HOT_UNLOAD\n");
}

APP_UPDATE
{
    os_event event;
    while (OS_GetNextEvent(&event))
    {
        if (event.type == OS_EventType_MousePress)
        {
            os->DebugPrint("Mouse clicked!\n");
        }
        else if (event.type == OS_EventType_MouseMove)
        {
            char buff[256];

            sprintf_s(buff, sizeof(buff), "x = %.02f, y = %.02f\n", os->mousePosition.x,
                      os->mousePosition.y);
            os->DebugPrint(buff);
        }
    }

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    os->SwapBuffers();
}

APP_CLOSE
{
    os->DebugPrint("APP_CLOSE\n");
}
