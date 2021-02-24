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

internal void SetupRenderer(Renderer *renderer)
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &renderer->maxTextureUnits);
    assert(renderer->maxTextureUnits == 32);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    TextureVertex vertices[] = {
        {v2(0, 1), v2(0, 1)},
        {v2(1, 0), v2(1, 0)},
        {v2(0, 0), v2(0, 0)},
        {v2(1, 1), v2(1, 1)},
    };

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // a_Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), 0);
    glEnableVertexAttribArray(1); // a_TexCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex),
                          (void *)offsetof(TextureVertex, texCoord));

    u32 indices[] = {
        0, 1, 2, // top left triangle
        0, 3, 1  // bottom right triangle
    };

    glGenBuffers(1, &renderer->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    {
        renderer->program = CreateShaderProgram(QUAD_VERT, SPRITE_FRAG);
        glUseProgram(renderer->program);
        glUniform1i(glGetUniformLocation(renderer->program, "u_Image"), 1);

        renderer->u_model = glGetUniformLocation(renderer->program, "u_Model");

        UpdateProjections(renderer);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(0);

    GL_CheckForErrors();
}

internal void DestroyRenderer(Renderer *renderer)
{
    glDeleteProgram(renderer->program);

    glDeleteBuffers(1, &renderer->vbo);
    glDeleteBuffers(1, &renderer->ibo);
    glDeleteVertexArrays(1, &renderer->vao);
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

internal void DrawTexture(Texture sprite, v2 position, f32 rotation, v2 scale, v2 origin)
{
    GLuint program = app->renderer.program;
    glUseProgram(program);

    v2 area = v2(sprite.width * scale.x, sprite.height * scale.y);
    v2 translate = position - (origin * area);
    m4 model = M4Identity();
    model *= M4Translate(v3(translate.x, translate.y, 0.0f));

    model *= M4Translate(v3(0.5f * area.x, 0.5f * area.y, 0.0f));
    model *= M4RotateZ(rotation);
    model *= M4Translate(v3(-0.5f * area.x, -0.5f * area.y, 0.0f));

    model *= M4Scale(v3(area.x, area.y, 1.0f));

    glUniformMatrix4fv(app->renderer.u_model, 1, 0, model.flatten);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sprite.id);

    glBindVertexArray(app->renderer.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->renderer.ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#if 0
internal inline Texture Texture_Load(SDL_Surface *surface)
{
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(app->renderer, surface);
    assert(imageTexture);

    Texture tex;
    tex.texture = imageTexture;
    tex.width = surface->w;
    tex.height = surface->h;

    SDL_FreeSurface(surface);

    return tex;
}

internal inline Texture Texture_LoadFromFontSolid(TTF_Font *font, const char *text)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, {0xFF, 0xFF, 0xFF, 0xFF});
    if (!textSurface)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TTF_RenderText_Solid", TTF_GetError(),
                                 NULL);
        *(int *)0 = 0;
    }

    return Texture_Load(textSurface);
}

internal inline Texture Texture_LoadFromImage(const char *imagePath)
{
    SDL_Surface *imageSurface = IMG_Load(imagePath);
    if (!imageSurface)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG_Load", IMG_GetError(), NULL);
        *(int *)0 = 0;
    }

    return Texture_Load(imageSurface);
}

internal inline void Texture_DrawWorld(Texture *tex, i32 x, i32 y)
{
    SDL_Rect rect = {x - (i32)app->camera.x, y - (i32)app->camera.y, tex->width * DRAW_SCALE,
                     tex->height * DRAW_SCALE};
    SDL_RenderCopy(app->renderer, tex->texture, NULL, &rect);
}

internal inline void Texture_DrawScreen(Texture *tex, i32 x, i32 y)
{
    SDL_Rect rect = {x, y, tex->width, tex->height};
    SDL_RenderCopy(app->renderer, tex->texture, NULL, &rect);
}

internal inline void Font_RenderText(TTF_Font *font, i32 x, i32 y, const char *formatString, ...)
{
    char buff[2048];
    va_list args;
    va_start(args, formatString);
    vsnprintf(buff, 2048, formatString, args);
    va_end(args);

    Texture t = Texture_LoadFromFontSolid(font, buff);
    Texture_DrawScreen(&t, x, y);
    SDL_DestroyTexture(t.texture);
}

internal inline v2 Texture_ToV2(Texture *texture)
{
    return {(f32)texture->width, (f32)texture->height};
}

internal void Tilemap_Draw(Tilemap *map, i32 xOffset, i32 yOffset)
{
    const i32 translate = map->tileWidth * DRAW_SCALE;
    const i32 tileColumns = map->atlas.width / map->tileWidth;

    if (app->debug)
    {
        SDL_SetRenderDrawColor(app->renderer, 0xFF, 0x00, 0x00, 0xFF);
    }

    for (i32 y = 0; y < map->height; ++y)
    {
        for (i32 x = 0; x < map->width; ++x)
        {
            SDL_Rect dest = {x * translate + xOffset - (i32)app->camera.x,
                             y * translate + yOffset - (i32)app->camera.y, translate, translate};

            if (dest.y > SCREEN_HEIGHT)
            {
                return;
            }

            if (dest.x > SCREEN_WIDTH || dest.y + dest.h <= 0)
            {
                break;
            }

            if (dest.x + dest.w <= 0)
            {
                continue;
            }

            i32 index = map->indices[(y * map->width) + x];
            // tiled export increments by 1 so we minus 1
            i32 tile = (index & 0xFFFFFFF) - 1;

            SDL_RendererFlip flip;
            if (index & 0x80000000)
            {
                flip = SDL_FLIP_HORIZONTAL;
            }
            else if (index & 0x40000000)
            {
                flip = SDL_FLIP_VERTICAL;
            }
            else if (index & 0x20000000)
            {
                // diagonal
                flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else
            {
                flip = SDL_FLIP_NONE;
            }

            SDL_Rect src = {(tile % tileColumns) * map->tileWidth,
                            (tile / tileColumns) * map->tileWidth, map->tileWidth, map->tileWidth};

            SDL_RenderCopyEx(app->renderer, map->atlas.texture, &src, &dest, 0, NULL, flip);

            if (app->debug && map->collision[(y * map->width) + x])
            {
                SDL_RenderDrawRect(app->renderer, &dest);
            }
        }
    }
}
#endif
