#include "app.h"

// Shader sources
const GLchar *vertexSource = "glsl(\n"
                             " #version 150 core\n"
                             " in vec2 position;\n"
                             " void main()\n"
                             " {\n"
                             "  gl_Position = vec4(position, 0.0, 1.0);\n"
                             " }\n"
                             ")glsl\n";

const GLchar *fragmentSource = "glsl(\n"
                               " #version 150 core\n"
                               " out vec4 outColor;\n"
                               " void main()\n"
                               " {\n"
                               "  outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                               " }\n"
                               ")glsl\n";

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("APP_PERMANENT_LOAD\n");
    LoadOpenGLProcedures();

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
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

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a triangle from the 3 vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap buffers
    os->SwapBuffers();
}

APP_CLOSE
{
    os->DebugPrint("APP_CLOSE\n");
}
