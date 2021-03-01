internal GLuint CompileGLSL(u32 type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    i32 result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        i32 length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char message[512];
        length = Max((u32)length, sizeof(message));
        glGetShaderInfoLog(shader, length, NULL, message);

        DisplayError("Cannot compile shader: %s\n", message);
        fprintf(stderr, "%s\n", source);
        assert(0);
    }

    return shader;
}

internal GLuint CreateShaderProgram(const char *vert, const char *frag)
{
    GLuint program = glCreateProgram();
    GLuint vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    GLuint fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

internal void UpdateProjections(Renderer *renderer)
{
    glUseProgram(renderer->program);

    m4 projection =
        M4Orthographic(0.0f, (f32)app->windowWidth, (f32)app->windowHeight, 0.0f, -1.0f, 1.0f);
    GLint u_Projection = glGetUniformLocation(renderer->program, "u_Projection");
    glUniformMatrix4fv(u_Projection, 1, 0, projection.flatten);
}

internal void CreateRenderer(Renderer *renderer)
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &renderer->maxTextureUnits);
    assert(renderer->maxTextureUnits == 32);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TextureVertex) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex),
                          (void *)offsetof(TextureVertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex),
                          (void *)offsetof(TextureVertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(TextureVertex),
                          (void *)offsetof(TextureVertex, texIndex));

    u32 *indices = (u32 *)malloc(sizeof(u32) * MAX_INDICES);
    assert(indices);
    for (i32 i = 0, stride = 0; i < MAX_INDICES; i += 6, stride += 4)
    {
        indices[i + 0] = stride + 0;
        indices[i + 1] = stride + 1;
        indices[i + 2] = stride + 2;

        indices[i + 3] = stride + 0;
        indices[i + 4] = stride + 3;
        indices[i + 5] = stride + 1;
    }

    glGenBuffers(1, &renderer->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_INDICES, indices, GL_STATIC_DRAW);
    free(indices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    renderer->program = CreateShaderProgram(TEXTURE_VERT, TEXTURE_FRAG);
    glUseProgram(renderer->program);

    i32 *samplers = (i32 *)malloc(sizeof(i32) * renderer->maxTextureUnits);
    assert(samplers);
    for (i32 i = 0; i < renderer->maxTextureUnits; ++i)
    {
        samplers[i] = i;
    }
    glUniform1iv(glGetUniformLocation(renderer->program, "u_Textures"), renderer->maxTextureUnits,
                 samplers);
    free(samplers);

    renderer->u_View = glGetUniformLocation(renderer->program, "u_View");

    UpdateProjections(renderer);

    glUseProgram(0);

    renderer->textureIDs = (GLuint *)malloc(sizeof(GLuint) * renderer->maxTextureUnits);
    assert(renderer->textureIDs);

    renderer->vertices = (TextureVertex *)malloc(sizeof(TextureVertex) * MAX_VERTICES);
    assert(renderer->vertices);

    GL_CheckForErrors();
}

internal void DestroyRenderer(Renderer *renderer)
{
    glDeleteProgram(renderer->program);

    glDeleteBuffers(1, &renderer->vbo);
    glDeleteBuffers(1, &renderer->ibo);
    glDeleteVertexArrays(1, &renderer->vao);

    free(renderer->vertices);
}

internal Texture CreateTexture(const char *imagePath)
{
    Texture result;

    i32 channels;
    u8 *imageData = stbi_load(imagePath, &result.width, &result.height, &channels, 0);
    assert(imageData);

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, result.width, result.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    return result;
}

internal void BeginDraw(Renderer *renderer, v2 camera)
{
    glUseProgram(renderer->program);

    m4 view = M4Translate(v3(-camera.x, -camera.y, 0.0f));
    glUniformMatrix4fv(renderer->u_View, 1, 0, view.flatten);

    renderer->quadCount = 0;
    renderer->textureCount = 0;
    renderer->camera = camera;
}

internal void FlushRenderer(Renderer *renderer)
{
    if (renderer->quadCount == 0)
    {
        return;
    }

    for (i32 i = 0; i < renderer->textureCount; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderer->textureIDs[i]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, app->renderer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->quadCount * 4 * sizeof(TextureVertex),
                    renderer->vertices);

    glBindVertexArray(app->renderer.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->renderer.ibo);
    glDrawElements(GL_TRIANGLES, renderer->quadCount * 6, GL_UNSIGNED_INT, NULL);

    renderer->quadCount = 0;
    renderer->textureCount = 0;
}

internal f32 FindOrCreateTextureIndex(Renderer *renderer, GLuint textureID)
{
    i32 texIndex = -1;
    for (i32 i = 0; i < renderer->textureCount; ++i)
    {
        if (renderer->textureIDs[i] == textureID)
        {
            texIndex = i;
            break;
        }
    }

    if (texIndex == -1)
    {
        if (renderer->textureCount == renderer->maxTextureUnits)
        {
            FlushRenderer(renderer);
        }

        texIndex = renderer->textureCount++;
        renderer->textureIDs[texIndex] = textureID;
    }

    return (f32)texIndex;
}

internal void DrawTextureMat(Renderer *renderer, Texture texture, m4 transform)
{
    if (renderer->quadCount == MAX_QUADS)
    {
        FlushRenderer(renderer);
    }

    f32 texIndex = FindOrCreateTextureIndex(renderer, texture.id);

    TextureVertex vertices[] = {
        {V4MultiplyM4(v4(0, 1, 0, 1), transform).xy, v2(0, 1), texIndex},
        {V4MultiplyM4(v4(1, 0, 0, 1), transform).xy, v2(1, 0), texIndex},
        {V4MultiplyM4(v4(0, 0, 0, 1), transform).xy, v2(0, 0), texIndex},
        {V4MultiplyM4(v4(1, 1, 0, 1), transform).xy, v2(1, 1), texIndex},
    };

    memcpy(&renderer->vertices[renderer->quadCount * 4], vertices, sizeof(vertices));
    renderer->quadCount++;
}

internal void DrawTexture(Renderer *renderer, Texture texture, v2 position, f32 rotation)
{
    // best to avoid matrix multiplication.
    // am lazy to change. oh well.
    v2 area = v2((f32)texture.width * PIXEL_ART_SCALE, (f32)texture.height * PIXEL_ART_SCALE);
    m4 transform = M4Translate(v3(position.x, position.y, 0.0f));
    if (rotation != 0.0f)
    {
        transform = M4MultiplyM4(transform, M4Translate(v3(area.x * 0.5f, area.y * 0.5f, 0.0f)));
        transform = M4MultiplyM4(transform, M4RotateZ(rotation));
        transform = M4MultiplyM4(transform, M4Translate(v3(area.x * -0.5f, area.y * -0.5f, 0.0f)));
    }
    transform = M4MultiplyM4(transform, M4Scale(v3(area.x, area.y, 1.0f)));

    DrawTextureMat(renderer, texture, transform);
}

internal void CreateTilemap(Tilemap *map, const char *atlasPath, const char *mapDataPath)
{
    map->atlas = CreateTexture(atlasPath);

    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(mapDataPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    assert(!layer->next);

    map->width = layer->width;
    map->height = layer->height;
    map->tileWidth = tiledMap->tilewidth;
    assert(layer->data_count == map->width * map->height);

    map->vertexPositions = (v2 *)malloc(sizeof(v2) * layer->data_count * 4);
    assert(map->vertexPositions);
    map->texCoords = (v2 *)malloc(sizeof(v2) * layer->data_count * 4);
    assert(map->texCoords);

    i32 realTileWidth = map->tileWidth * PIXEL_ART_SCALE;
    i32 tileColumns = map->atlas.width / map->tileWidth;

    for (i32 y = 0; y < map->height; ++y)
    {
        for (i32 x = 0; x < map->width; ++x)
        {
            i32 index = layer->data[(y * map->width) + x];

            m4 transform = M4Translate(v3((f32)x * realTileWidth, (f32)y * realTileWidth, 0.0f));

            transform = M4MultiplyM4(
                transform, M4Translate(v3(realTileWidth * 0.5f, realTileWidth * 0.5f, 0.0f)));
            // flip horizontal
            if (index & 0x80000000)
            {
                transform = M4MultiplyM4(transform, M4Scale(v3(-1.0f, 1.0f, 1.0f)));
            }
            // flip vertical
            else if (index & 0x40000000)
            {
                transform = M4MultiplyM4(transform, M4Scale(v3(1.0f, -1.0f, 1.0f)));
            }
            // flip diagonal
            else if (index & 0x20000000)
            {
                transform = M4MultiplyM4(transform, M4Scale(v3(-1.0f, -1.0f, 1.0f)));
            }
            transform = M4MultiplyM4(
                transform, M4Translate(v3(realTileWidth * -0.5f, realTileWidth * -0.5f, 0.0f)));

            transform =
                M4MultiplyM4(transform, M4Scale(v3((f32)realTileWidth, (f32)realTileWidth, 1.0f)));

            v2 *quadPositions = &map->vertexPositions[((y * map->width) + x) * 4];
            quadPositions[0] = V4MultiplyM4(v4(0, 1, 0, 1), transform).xy;
            quadPositions[1] = V4MultiplyM4(v4(1, 0, 0, 1), transform).xy;
            quadPositions[2] = V4MultiplyM4(v4(0, 0, 0, 1), transform).xy;
            quadPositions[3] = V4MultiplyM4(v4(1, 1, 0, 1), transform).xy;

            i32 tile = (index & 0xFFFFFFF) - 1;
            v2 xy = v2((f32)(tile % tileColumns), (f32)(tile / tileColumns));
            v2 topLeft = v2(xy.x * map->tileWidth / map->atlas.width,
                            xy.y * map->tileWidth / map->atlas.height);
            v2 bottomRight = v2((xy.x + 1) * map->tileWidth / map->atlas.width,
                                (xy.y + 1) * map->tileWidth / map->atlas.height);

            v2 *quadTexCoords = &map->texCoords[((y * map->width) + x) * 4];
            quadTexCoords[0] = v2(topLeft.x, bottomRight.y);
            quadTexCoords[1] = v2(bottomRight.x, topLeft.y);
            quadTexCoords[2] = v2(topLeft.x, topLeft.y);
            quadTexCoords[3] = v2(bottomRight.x, bottomRight.y);
        }
    }
}

internal void DestroyTilemap(Tilemap *map)
{
    free(map->vertexPositions);
    free(map->texCoords);
}

internal void DrawTilemap(Renderer *renderer, Tilemap *map)
{
    f32 texIndex = FindOrCreateTextureIndex(renderer, map->atlas.id);
    i32 realTileWidth = map->tileWidth * PIXEL_ART_SCALE;

    for (i32 y = 0; y < map->height; ++y)
    {
        for (i32 x = 0; x < map->width; ++x)
        {
            if (y * realTileWidth > app->windowHeight + renderer->camera.y)
            {
                return;
            }

            if (x * realTileWidth > app->windowWidth + renderer->camera.x ||
                (y + 1) * realTileWidth <= renderer->camera.y)
            {
                break;
            }

            if ((x + 1) * realTileWidth <= renderer->camera.x)
            {
                continue;
            }

            if (renderer->quadCount == MAX_QUADS)
            {
                FlushRenderer(renderer);
            }

            for (i32 q = 0; q < 4; ++q)
            {
                TextureVertex *vertex = &renderer->vertices[renderer->quadCount * 4 + q];
                vertex->position = map->vertexPositions[(y * map->width + x) * 4 + q];
                vertex->texCoord = map->texCoords[(y * map->width + x) * 4 + q];
                vertex->texIndex = texIndex;
            }

            renderer->quadCount++;
        }
    }
}
