internal u32 CompileGLSL(u32 type, const char *source)
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
        char message[512];
        length = Max((u32)length, sizeof(message));
        glGetShaderInfoLog(shader, length, NULL, message);
        fprintf(stderr, "Cannot compile shader: %s\n", message);
        fprintf(stderr, "%s\n", source);
        assert(0);
    }

    return shader;
}

internal GLuint CreateShader(const char *vert, const char *frag)
{
    u32 shader = glCreateProgram();
    u32 vertexShader = CompileGLSL(GL_VERTEX_SHADER, vert);
    u32 fragmentShader = CompileGLSL(GL_FRAGMENT_SHADER, frag);

    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glValidateProgram(shader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

internal void SetupRenderer(Renderer *renderer)
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

    {
        renderer->spriteShader = CreateShader(QUAD_VERT, SPRITE_FRAG);
        glUseProgram(renderer->spriteShader);
        glUniform1i(glGetUniformLocation(renderer->spriteShader, "image"), 1);

        m4 projection = M4Orthographic(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(renderer->spriteShader, "projection"), 1, 0,
                           projection.flatten);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(0);
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

internal void DrawSpriteExt(Texture sprite, v2 position, f32 rotation, v2 scale, v2 origin)
{
    u32 shader = app->renderer.spriteShader;
    glUseProgram(shader);

    v2 area = v2(sprite.width * scale.x, sprite.height * scale.y);
    v2 translate = position - (origin * area);
    m4 model = M4Identity();
    model *= M4Translate(v3(translate.x, translate.y, 0.0f));

    model *= M4Translate(v3(0.5f * area.x, 0.5f * area.y, 0.0f));
    model *= M4RotateZ(rotation);
    model *= M4Translate(v3(-0.5f * area.x, -0.5f * area.y, 0.0f));

    model *= M4Scale(v3(area.x, area.y, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, 0, model.flatten);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sprite.id);

    glBindVertexArray(app->renderer.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*
internal void DrawSprite(Texture sprite, v2 position, f32 rotation)
{
    DrawSpriteExt(sprite, position, rotation, v2(1, 1), v2(0, 0));
}*/

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
