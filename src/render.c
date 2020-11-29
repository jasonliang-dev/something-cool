char *spriteVertexShaderSource = "                                    \n\
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

char *spriteFragmentShaderSource = "       \n\
    #version 330 core                      \n\
                                           \n\
    in vec2 TexCoords;                     \n\
    out vec4 color;                        \n\
                                           \n\
    uniform sampler2D image;               \n\
                                           \n\
    void main()                            \n\
    {                                      \n\
        color = texture(image, TexCoords); \n\
    }                                      \n\
";

global u32 globalSpriteShader;

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

internal void R_InitializeShaders()
{
    globalSpriteShader = R_CreateShader(spriteVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(globalSpriteShader);

    m4 projection =
        M4Ortho(0.0f, (f32)os->windowResolution.x, (f32)os->windowResolution.y, 0.0f, -1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(globalSpriteShader, "image"), 0);
    glUniformMatrix4fv(glGetUniformLocation(globalSpriteShader, "projection"), 1, 0,
                       (f32 *)projection.elements);
}

internal sprite_data R_CreateSprite(char *imagePath)
{
    sprite_data result;

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

    i32 width;
    i32 height;
    i32 channels;
    u8 *imageData = stbi_load(imagePath, &width, &height, &channels, 0);

    u32 texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    result.vao = vao;
    result.texture = texture;
    result.size.width = width;
    result.size.height = height;
    return result;
}

internal void R_DrawSprite(sprite_data sprite, v2 position, f32 rotation)
{
    glUseProgram(globalSpriteShader);

    m4 model = M4Identity();
    model = M4MultiplyM4(model, M4Translate(v3(position.x, position.y, 0.0f)));

    v2 size = V2FromIV2(sprite.size);
    model = M4MultiplyM4(model, M4Translate(v3(0.5f * size.x, 0.5f * size.y, 0.0f)));
    model = M4MultiplyM4(model, M4RotateZ(rotation));
    model = M4MultiplyM4(model, M4Translate(v3(-0.5f * size.x, -0.5f * size.y, 0.0f)));

    model = M4MultiplyM4(model, M4Scale(v3(size.x, size.y, 1.0f)));

    glUniformMatrix4fv(glGetUniformLocation(globalSpriteShader, "model"), 1, 0,
                       (f32 *)model.elements);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.texture);

    glBindVertexArray(sprite.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
