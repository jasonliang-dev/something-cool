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

internal void R_Pixel2DProjection(u32 shader)
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader, "projection"), 1, 0,
        M4Ortho(0.0f, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f).flatten);
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

internal void R_SetupRendering()
{
    GL_LoadProcedures();

    {
        f32 vertices[] = {
            // xy       tex
            0.0f, 1.0f, 0.0f, 1.0f, //
            1.0f, 0.0f, 1.0f, 0.0f, //
            0.0f, 0.0f, 0.0f, 0.0f, //

            0.0f, 1.0f, 0.0f, 1.0f, //
            1.0f, 1.0f, 1.0f, 1.0f, //
            1.0f, 0.0f, 1.0f, 0.0f  //
        };

        u32 vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &app->quadVAO);
        glBindVertexArray(app->quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(v4), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    {
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, LOW_RES_SCREEN_WIDTH,
                              LOW_RES_SCREEN_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glGenFramebuffers(1, &app->screenFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, app->screenFBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
        Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    app->shaders.sprite = R_InitShader(spriteVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(app->shaders.sprite);
    glUniform1i(glGetUniformLocation(app->shaders.sprite, "image"), 1);
    R_Pixel2DProjection(app->shaders.sprite);

    app->shaders.map = R_InitShader(tilemapVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(app->shaders.map);
    glUniform1i(glGetUniformLocation(app->shaders.map, "atlas"), 2);
    R_Pixel2DProjection(app->shaders.map);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT);
}

internal texture_t R_CreateTexture(char *imagePath)
{
    texture_t result;

    i32 channels;
    u8 *imageData = stbi_load(imagePath, (i32 *)&result.width, (i32 *)&result.height, &channels, 0);

    if (!imageData)
    {
        OS_DisplayError("Cannot display image: \"%s\"\n", imagePath);
    }

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

internal void R_DrawSprite(texture_t sprite, v2 position, f32 rotation)
{
    glUseProgram(app->shaders.sprite);

    v2 origin = v2(0.5f, 0.5f);
    m4 model = M4Identity();
    model = M4MultiplyM4(model, M4Translate(v3(position.x - (origin.x * sprite.width),
                                               position.y - (origin.y * sprite.height), 0.0f)));

    model = M4MultiplyM4(model, M4Translate(v3(0.5f * sprite.width, 0.5f * sprite.height, 0.0f)));
    model = M4MultiplyM4(model, M4RotateZ(rotation));
    model = M4MultiplyM4(model, M4Translate(v3(-0.5f * sprite.width, -0.5f * sprite.height, 0.0f)));

    model = M4MultiplyM4(model, M4Scale(v3((f32)sprite.width, (f32)sprite.height, 1.0f)));

    glUniformMatrix4fv(glGetUniformLocation(app->shaders.sprite, "model"), 1, 0, model.flatten);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);

    glBindVertexArray(app->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

internal tilemap_t R_CreateTilemap(char *jsonPath, texture_t atlas)
{
    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(jsonPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    Assert(layer->next == NULL);

    tilemap_t result;
    result.cols = layer->width;
    result.rows = layer->height;
    result.atlas = atlas;
    result.tileSize = tiledMap->tilewidth;

    u32 indexSize = sizeof(v2) * layer->data_count;
    v2 *atlasIndex = M_ArenaPushZero(&app->scratchArena, indexSize);
    u32 atlasColumnCount = atlas.width / result.tileSize;

    for (i32 i = 0; i < layer->data_count; i++)
    {
        // -1 because exported file increments data by 1. bruh why???
        u32 tile = layer->data[i] - 1;
        u32 column = (tile % atlasColumnCount);
        u32 row = (tile / atlasColumnCount);

        atlasIndex[i] = v2((f32)column, (f32)row);
    }

    glBindVertexArray(app->quadVAO);

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
    M_ArenaPop(&app->scratchArena, indexSize);

    return result;
}

internal void R_DrawTilemap(tilemap_t map)
{
    glUseProgram(app->shaders.map);

    m4 model = M4Scale(v3((f32)map.tileSize, (f32)map.tileSize, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(app->shaders.map, "model"), 1, 0, model.flatten);
    glUniform2i(glGetUniformLocation(app->shaders.map, "mapSize"), map.cols, map.rows);
    glUniform2f(glGetUniformLocation(app->shaders.map, "atlasSize"),
                1.0f * map.atlas.width / map.tileSize, 1.0f * map.atlas.height / map.tileSize);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, map.atlas.textureID);

    glBindVertexArray(app->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, map.cols * map.rows);
    glBindVertexArray(0);
}
