char *quadVertexShaderSource = "                                      \n\
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

internal u32 R_CompileShader(u32 type, const char *source)
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
        OS_DisplayError(message);

        return 0;
    }

    return shader;
}

internal void R_UpdateSpriteProjection(u32 spriteShader)
{
    m4 projection =
        M4Ortho(0.0f, (f32)os->windowResolution.x, (f32)os->windowResolution.y, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(spriteShader, "projection"), 1, 0,
                       (f32 *)projection.elements);
}

internal u32 R_InitShader(char *vertexSource, char *fragmentSource)
{
    u32 spriteShader = glCreateProgram();
    u32 vertexShader = R_CompileShader(GL_VERTEX_SHADER, vertexSource);
    u32 fragmentShader = R_CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(spriteShader, vertexShader);
    glAttachShader(spriteShader, fragmentShader);
    glLinkProgram(spriteShader);
    glValidateProgram(spriteShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(spriteShader);

    R_UpdateSpriteProjection(spriteShader);
    glUniform1i(glGetUniformLocation(spriteShader, "image"), 0);

    return spriteShader;
}

internal u32 R_CreateQuadVAO(void)
{
    v4 vertices[] = {
        // position    texture
        v4(0.0f, 1.0f, 0.0f, 1.0f), //
        v4(1.0f, 0.0f, 1.0f, 0.0f), //
        v4(0.0f, 0.0f, 0.0f, 0.0f), //

        v4(0.0f, 1.0f, 0.0f, 1.0f), //
        v4(1.0f, 1.0f, 1.0f, 1.0f), //
        v4(1.0f, 0.0f, 1.0f, 0.0f)  //
    };

    u32 vao;
    u32 vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(v4), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

internal texture R_CreateTexture(char *imagePath)
{
    texture result;

    i32 channels;
    u8 *imageData = stbi_load(imagePath, (i32 *)&result.width, (i32 *)&result.height, &channels, 0);

    glGenTextures(1, &result.textureID);

    glBindTexture(GL_TEXTURE_2D, result.textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    return result;
}

internal void R_DrawSprite(u32 spriteShader, u32 spriteVAO, texture sprite, v2 position,
                           f32 rotation)
{
    glUseProgram(spriteShader);

    m4 model = M4Identity();
    model = M4MultiplyM4(model, M4Translate(v3(position.x, position.y, 0.0f)));

    model = M4MultiplyM4(model, M4Translate(v3(0.5f * sprite.width, 0.5f * sprite.height, 0.0f)));
    model = M4MultiplyM4(model, M4RotateZ(rotation));
    model = M4MultiplyM4(model, M4Translate(v3(-0.5f * sprite.width, -0.5f * sprite.height, 0.0f)));

    model = M4MultiplyM4(model, M4Scale(v3((f32)sprite.width, (f32)sprite.height, 1.0f)));

    glUniformMatrix4fv(glGetUniformLocation(spriteShader, "model"), 1, 0, model.elements[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);

    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

internal tilemap *R_LoadTilemap(char *jsonPath, u32 atlas)
{
    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(jsonPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    Assert(layer->next == NULL);

    tilemap *result =
        M_ArenaPush(&os->permanentArena, sizeof(tilemap) + (sizeof(u32) * layer->data_count));

    result->width = layer->width;
    result->height = layer->height;
    result->atlas = atlas;
    result->dataLength = layer->data_count;
    MemoryCopy(result->data, layer->data, sizeof(u32) * layer->data_count);

    cute_tiled_free_map(tiledMap);

    return result;
}
