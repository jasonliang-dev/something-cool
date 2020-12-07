char quadVertexShaderSource[] = "                                     \n\
    #version 330 core                                                 \n\
                                                                      \n\
    layout (location = 0) in vec4 vertex;                             \n\
                                                                      \n\
    out vec2 texCoords;                                               \n\
                                                                      \n\
    uniform mat4 model;                                               \n\
    uniform mat4 projection;                                          \n\
                                                                      \n\
    void main()                                                       \n\
    {                                                                 \n\
        texCoords = vertex.zw;                                        \n\
        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n\
    }                                                                 \n\
";

char spriteFragmentShaderSource[] = "      \n\
    #version 330 core                      \n\
                                           \n\
    in vec2 texCoords;                     \n\
    out vec4 color;                        \n\
                                           \n\
    uniform sampler2D image;               \n\
                                           \n\
    void main()                            \n\
    {                                      \n\
        color = texture(image, texCoords); \n\
    }                                      \n\
";

char tilemapVertexShaderSource[] = "                                                       \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    layout (location = 0) in vec4 vertex;                                                  \n\
    layout (location = 1) in vec2 tileIndex;                                               \n\
                                                                                           \n\
    out vec2 texCoords;                                                                    \n\
    out vec2 tileCoords;                                                                   \n\
                                                                                           \n\
    uniform mat4 model;                                                                    \n\
    uniform mat4 projection;                                                               \n\
    uniform ivec2 mapSize;                                                                 \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        texCoords = vertex.zw;                                                             \n\
        tileCoords = tileIndex;                                                            \n\
        ivec2 tileOffset = ivec2(gl_InstanceID % mapSize.x, gl_InstanceID / mapSize.x);    \n\
        gl_Position = projection * model * vec4(vertex.xy + (1.0 * tileOffset), 0.0, 1.0); \n\
    }                                                                                      \n\
";

char tilemapFragmentShaderSource[] = "       \n\
    #version 330 core                        \n\
                                             \n\
    in vec2 texCoords;                       \n\
    in vec2 tileCoords;                      \n\
    out vec4 color;                          \n\
                                             \n\
    uniform sampler2D atlas;                 \n\
    uniform vec2 atlasSize;                  \n\
                                             \n\
    void main()                              \n\
    {                                        \n\
        vec2 uv = tileCoords / atlasSize;    \n\
        vec2 offset = texCoords / atlasSize; \n\
        color = texture(atlas, uv + offset); \n\
    }                                        \n\
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

internal void R_Update2DProjection(u32 shader, u32 scale)
{
    m4 projection =
        M4Ortho(0.0f, (f32)os->windowResolution.x, (f32)os->windowResolution.y, 0.0f, -1.0f, 1.0f);
    projection = M4MultiplyM4(projection, M4Scale(v3((f32)scale, (f32)scale, 1.0f)));

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, 0, projection.flatten);
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

    return spriteShader;
}

internal u32 R_CreateQuadVAO(void)
{
    f32 vertices[] = {
        // xy       uv
        0.0f, 1.0f, 0.0f, 1.0f, //
        1.0f, 0.0f, 1.0f, 0.0f, //
        0.0f, 0.0f, 0.0f, 0.0f, //

        0.0f, 1.0f, 0.0f, 1.0f, //
        1.0f, 1.0f, 1.0f, 1.0f, //
        1.0f, 0.0f, 1.0f, 0.0f  //
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

internal texture_t R_CreateTexture(char *imagePath)
{
    texture_t result;

    i32 channels;
    u8 *imageData = stbi_load(imagePath, (i32 *)&result.width, (i32 *)&result.height, &channels, 0);

    glGenTextures(1, &result.textureID);
    glBindTexture(GL_TEXTURE_2D, result.textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    return result;
}

internal void R_DrawSprite(u32 spriteShader, u32 spriteVAO, texture_t sprite, v2 position,
                           f32 rotation)
{
    glUseProgram(spriteShader);

    m4 model = M4Identity();
    model = M4MultiplyM4(model, M4Translate(v3(position.x, position.y, 0.0f)));

    model = M4MultiplyM4(model, M4Translate(v3(0.5f * sprite.width, 0.5f * sprite.height, 0.0f)));
    model = M4MultiplyM4(model, M4RotateZ(rotation));
    model = M4MultiplyM4(model, M4Translate(v3(-0.5f * sprite.width, -0.5f * sprite.height, 0.0f)));

    model = M4MultiplyM4(model, M4Scale(v3((f32)sprite.width, (f32)sprite.height, 1.0f)));

    glUniformMatrix4fv(glGetUniformLocation(spriteShader, "model"), 1, 0, model.flatten);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);

    glBindVertexArray(spriteVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

internal tilemap_t R_CreateTilemap(char *jsonPath, texture_t atlas, u32 quadVAO)
{
    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(jsonPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    Assert(layer->next == NULL);

    tilemap_t result;
    result.width = layer->width;
    result.height = layer->height;
    result.atlas = atlas;
    result.tileSize = tiledMap->tilewidth;

    u32 indexSize = sizeof(v2) * layer->data_count;
    v2 *atlasIndex = M_ArenaPushZero(&app.scratchArena, indexSize);
    u32 atlasColumnCount = atlas.width / result.tileSize;

    for (i32 i = 0; i < layer->data_count; i++)
    {
        // -1 because exported file increments data by 1. bruh why???
        u32 tile = layer->data[i] - 1;
        u32 column = (tile % atlasColumnCount);
        u32 row = (tile / atlasColumnCount);

        atlasIndex[i] = v2((f32)column, (f32)row);
    }

    glBindVertexArray(quadVAO);

    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, indexSize, atlasIndex, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v2), 0);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    cute_tiled_free_map(tiledMap);
    M_ArenaPop(&app.scratchArena, indexSize);

    return result;
}

internal void R_DrawTilemap(u32 mapShader, u32 quadVAO, tilemap_t map)
{
    glUseProgram(mapShader);

    m4 model = M4Scale(v3((f32)map.tileSize, (f32)map.tileSize, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(mapShader, "model"), 1, 0, model.flatten);
    glUniform2i(glGetUniformLocation(mapShader, "mapSize"), map.width, map.height);
    glUniform2f(glGetUniformLocation(mapShader, "atlasSize"), 1.0f * map.atlas.width / map.tileSize,
                1.0f * map.atlas.height / map.tileSize);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, map.atlas.textureID);

    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, map.width * map.height);
    glBindVertexArray(0);
}
