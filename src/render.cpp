internal u32 Render_CompileShader(u32 type, const char *source)
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
        length = Max((u32)length, sizeof(message));
        glGetShaderInfoLog(shader, length, NULL, message);
        OS_DisplayError(message);

        return 0;
    }

    return shader;
}

internal void Render_OrthoProjection(u32 shader, v2 resolution)
{
    i32 loc = glGetUniformLocation(shader, "projection");
    m4 projection = M4Ortho(0.0f, resolution.width, resolution.height, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(loc, 1, 0, projection.flatten);
}

internal u32 Render_InitShader(char *vertexSource, char *fragmentSource)
{
    u32 shader = glCreateProgram();
    u32 vertexShader = Render_CompileShader(GL_VERTEX_SHADER, vertexSource);
    u32 fragmentShader = Render_CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glValidateProgram(shader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

internal void Render_SetupRendering(renderer_t *renderer)
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

        glGenVertexArrays(1, &renderer->quadVAO);
        glBindVertexArray(renderer->quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(f32) * 4, 0);

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

        glGenFramebuffers(1, &renderer->pixelFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer->pixelFBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
        Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    shaders_t *shaders = &renderer->shaders;
    shaders->quad = Render_InitShader(quadVertexShaderSource, quadFragmentShaderSource);
    glUseProgram(shaders->quad);
    Render_OrthoProjection(shaders->quad, v2(LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT));

    shaders->sprite = Render_InitShader(quadVertexShaderSource, spriteFragmentShaderSource);
    glUseProgram(shaders->sprite);
    glUniform1i(glGetUniformLocation(shaders->sprite, "image"), TEXTURE_UNIT_SPRITE);
    Render_OrthoProjection(shaders->sprite, v2(LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT));

    shaders->tilemap = Render_InitShader(tilemapVertexShaderSource, tilemapFragmentShaderSource);
    glUseProgram(shaders->tilemap);
    glUniform1i(glGetUniformLocation(shaders->tilemap, "atlas"), TEXTURE_UNIT_TILEMAP);
    Render_OrthoProjection(shaders->tilemap, v2(LOW_RES_SCREEN_WIDTH, LOW_RES_SCREEN_HEIGHT));

    shaders->font = Render_InitShader(quadVertexShaderSource, fontFragmentShaderSource);
    glUseProgram(shaders->font);
    glUniform1i(glGetUniformLocation(shaders->font, "bitmap"), TEXTURE_UNIT_FONT);
    Render_OrthoProjection(shaders->font, v2((f32)os->windowWidth, (f32)os->windowHeight));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(0);
}

internal void Render_CreateTexture(char *imagePath, texture_t *result)
{
    i32 channels;
    u8 *imageData =
        stbi_load(imagePath, (i32 *)&result->width, (i32 *)&result->height, &channels, 0);

    if (!imageData)
    {
        OS_DisplayError("Cannot display image: \"%s\"\n", imagePath);
    }

    glGenTextures(1, &result->textureID);
    glBindTexture(GL_TEXTURE_2D, result->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, result->width, result->height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);
}

internal void Render_DrawRect(v4 color, v2 position, v2 size)
{
    renderer_t *renderer = &app->renderer;

    glUseProgram(renderer->shaders.quad);

    m4 model = M4Identity();
    model *= M4Translate(v3(position.x, position.y, 0.0f));
    model *= M4Scale(v3(size.x, size.y, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders.quad, "model"), 1, 0, model.flatten);
    glUniform4fv(glGetUniformLocation(renderer->shaders.quad, "drawColor"), 1, color.elements);

    glBindVertexArray(renderer->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

internal void Render_DrawSpriteExt(texture_t sprite, v2 position, f32 rotation, v2 scale, v2 origin)
{
    renderer_t *renderer = &app->renderer;

    glUseProgram(renderer->shaders.sprite);

    v2 area = v2(sprite.width * scale.x, sprite.height * scale.y);
    v2 translate = position - (origin * area);
    m4 model = M4Identity();
    model *= M4Translate(v3(translate.x, translate.y, 0.0f));

    model *= M4Translate(v3(0.5f * area.x, 0.5f * area.y, 0.0f));
    model *= M4RotateZ(rotation);
    model *= M4Translate(v3(-0.5f * area.x, -0.5f * area.y, 0.0f));

    model *= M4Scale(v3(area.x, area.y, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders.sprite, "model"), 1, 0,
                       model.flatten);

    glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_SPRITE);
    glBindTexture(GL_TEXTURE_2D, sprite.textureID);

    glBindVertexArray(renderer->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

internal void Render_DrawSprite(texture_t sprite, v2 position, f32 rotation)
{
    Render_DrawSpriteExt(sprite, position, rotation, v2(1, 1), v2(0.5f, 0.5f));
}

internal void Render_CreateTilemap(char *jsonPath, texture_t atlas, tilemap_t *result)
{
    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(jsonPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    Assert(layer->next == NULL);

    result->cols = layer->width;
    result->rows = layer->height;
    result->atlas = atlas;
    result->tileSize = tiledMap->tilewidth;

    u32 indexSize = sizeof(v2) * layer->data_count;
    v2 *atlasIndex = (v2 *)Memory_ArenaPushZero(&app->scratchArena, indexSize);
    u32 atlasColumnCount = atlas.width / result->tileSize;

    for (i32 i = 0; i < layer->data_count; i++)
    {
        // -1 because exported file increments data by 1. bruh why???
        u32 tile = layer->data[i] - 1;
        u32 column = tile % atlasColumnCount;
        u32 row = tile / atlasColumnCount;

        atlasIndex[i] = v2((f32)column, (f32)row);
    }

    glBindVertexArray(app->renderer.quadVAO);

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
    Memory_ArenaPop(&app->scratchArena, indexSize);
}

internal void Render_DrawTilemap(tilemap_t map, v2 position)
{
    renderer_t *renderer = &app->renderer;

    glUseProgram(renderer->shaders.tilemap);

    m4 model = M4Identity();
    model *= M4Translate(v3(position.x, position.y, 0.0f));
    model *= M4Scale(v3((f32)map.tileSize, (f32)map.tileSize, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders.tilemap, "model"), 1, 0,
                       model.flatten);
    glUniform2i(glGetUniformLocation(renderer->shaders.tilemap, "mapSize"), map.cols, map.rows);
    glUniform2f(glGetUniformLocation(renderer->shaders.tilemap, "atlasSize"),
                1.0f * map.atlas.width / map.tileSize, 1.0f * map.atlas.height / map.tileSize);

    glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_TILEMAP);
    glBindTexture(GL_TEXTURE_2D, map.atlas.textureID);

    glBindVertexArray(renderer->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, map.cols * map.rows);
    glBindVertexArray(0);
}

internal void Render_CreateFont(char *filePath, font_t *font)
{
    u32 bitmapWidth = 512;
    u32 bitmapHeight = 512;
    f32 fontSize = 32.0f;

    u8 *bitmap = (u8 *)Memory_ArenaPush(&app->scratchArena, Megabytes(1));
    u8 *ttfBuffer = NULL;
    u32 ttfBufferLen = 0;
    OS_ReadFile(&app->scratchArena, filePath, (void **)&ttfBuffer, &ttfBufferLen);

    stbtt_BakeFontBitmap(ttfBuffer, 0, fontSize, bitmap, bitmapWidth, bitmapHeight, ' ',
                         ArrayCount(font->bakedCharData), font->bakedCharData);

    glGenTextures(1, &font->textureID);
    glBindTexture(GL_TEXTURE_2D, font->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, bitmapWidth, bitmapHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
                 bitmap);

    glBindTexture(GL_TEXTURE_2D, 0);
    Memory_ArenaPop(&app->scratchArena, Megabytes(1) + ttfBufferLen);
}

internal void Render_DrawText(font_t *font, v2 position, char *text)
{
    (void)text;
    renderer_t *renderer = &app->renderer;

    glUseProgram(renderer->shaders.font);

    m4 model = M4Identity();
    model *= M4Translate(v3(position.x, position.y, 0.0f));
    model *= M4Scale(v3(512.0f, 512.0f, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaders.font, "model"), 1, 0, model.flatten);

    glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT_FONT);
    glBindTexture(GL_TEXTURE_2D, font->textureID);

    glBindVertexArray(renderer->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
