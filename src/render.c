global char *globalSpriteVertexShader = "                             \n\
    #version 330 core                                                 \n\
                                                                      \n\
    layout (location = 0) in vec4 vertex;                             \n\
                                                                      \n\
    out vec2 TexCoords;                                               \n\
    uniform mat4 model;                                               \n\
    uniform mat4 projection;                                          \n\
                                                                      \n\
    void main()                                                       \n\
    {                                                                 \n\
        TexCoords = vertex.zw;                                        \n\
        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n\
    }                                                                 \n\
";

global char *globalSpriteFragmentShader = "                         \n\
    #version 330 core                                               \n\
                                                                    \n\
    in vec2 TexCoords;                                              \n\
    out vec4 color;                                                 \n\
                                                                    \n\
    uniform sampler2D image;                                        \n\
    uniform vec3 spriteColor;                                       \n\
                                                                    \n\
    void main()                                                     \n\
    {                                                               \n\
        color = vec4(spriteColor, 1.0) * texture(image, TexCoords); \n\
    }                                                               \n\
";

internal u32 R_CompileShader(u32 type, char *source)
{
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    i32 result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        i32 length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char message[512] = {0};
        length = Max(length, sizeof(message));
        glGetShaderInfoLog(shader, length, NULL, message);
        os->DebugDisplayError(message);

        return 0;
    }

    return shader;
}

internal u32 R_CreateShader(char *vertexSource, char *fragmentSource)
{
    u32 program = glCreateProgram();
    u32 vertexShader = R_CompileShader(GL_VERTEX_SHADER, vertexSource);
    u32 fragmentShader = R_CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

internal u32 R_CreateTextureFromFile(char *path)
{
    i32 width;
    i32 height;
    i32 channels;
    u8 *data = stbi_load(path, &width, &height, &channels, 0);

    u32 texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return texture;
}

internal u32 R_InitSpriteRenderer()
{
    u32 vao;
    u32 vbo;

    v2 vertices[] = {
        // position     texture
        v2(0.0f, 1.0f), v2(0.0f, 1.0f), //
        v2(1.0f, 0.0f), v2(1.0f, 0.0f), //
        v2(0.0f, 0.0f), v2(0.0f, 0.0f), //

        v2(0.0f, 1.0f), v2(0.0f, 1.0f), //
        v2(1.0f, 1.0f), v2(1.0f, 1.0f), //
        v2(1.0f, 0.0f), v2(1.0f, 0.0f)  //
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(v2) * 2, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

internal void R_DrawSprite(u32 spriteVAO, u32 shader, u32 texture, v2 position, v2 scale,
                           f32 rotation, v3 color)
{
    glUseProgram(shader);
    m4 model = M4Identity();
    model = M4MultiplyM4(model, M4Translate(v3(position.x, position.y, 0.0f)));

    (void)rotation;
    model = model; // idk how to rotate lol

    model = M4MultiplyM4(model, M4Scale(v3(scale.x, scale.y, 1.0f)));

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, 0, (f32 *)model.elements);
    glUniform3f(glGetUniformLocation(shader, "spriteColor"), color.r, color.g, color.b);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
