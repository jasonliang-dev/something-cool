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
