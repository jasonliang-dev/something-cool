internal Texture Texture_Load(const char *imagePath)
{
    SDL_Surface *imageSurface = IMG_Load(imagePath);
    if (!imageSurface)
    {
        fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
        *(int *)0 = 0;
    }

    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(app->renderer, imageSurface);
    assert(imageTexture);

    Texture sprite;
    sprite.texture = imageTexture;
    sprite.width = imageSurface->w;
    sprite.height = imageSurface->h;

    SDL_FreeSurface(imageSurface);

    return sprite;
}

internal inline void Texture_Draw(Texture *s, i32 x, i32 y)
{
    SDL_Rect rect = {x - (i32)app->camera.x, y - (i32)app->camera.y, s->width * app->scale,
                     s->height * app->scale};
    SDL_RenderCopy(app->renderer, s->texture, NULL, &rect);
}

internal void Tilemap_Load(Tilemap *map, const char *imagePath, const char *tiledPath)
{
    map->atlas = Texture_Load(imagePath);

    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(tiledPath, 0);
    cute_tiled_layer_t *layer = tiledMap->layers;
    assert(!layer->next);

    map->width = layer->width;
    map->height = layer->height;
    map->tileSize = tiledMap->tilewidth;

    map->tiles = (i32 *)malloc(sizeof(i32) * layer->data_count);
    assert(map->tiles);
    memcpy(map->tiles, layer->data, sizeof(i32) * layer->data_count);

    cute_tiled_free_map(tiledMap);
}

internal void Tilemap_Draw(Tilemap *map, i32 xOffset, i32 yOffset)
{
    const i32 translate = map->tileSize * app->scale;
    const i32 tileColumns = map->atlas.width / map->tileSize;

    i32 tile;
    i32 atlasLookup;

    SDL_RendererFlip flip;
    SDL_Rect src;
    SDL_Rect dest;

    for (i32 y = 0; y < map->height; y++)
    {
        for (i32 x = 0; x < map->width; x++)
        {
            dest = {x * translate + xOffset - (i32)app->camera.x,
                    y * translate + yOffset - (i32)app->camera.y, translate, translate};

            if (dest.y > SCREEN_HEIGHT)
            {
                return;
            }

            if (dest.x > SCREEN_WIDTH || dest.y + dest.h <= 0)
            {
                goto nextRow;
            }

            if (dest.x + dest.w <= 0)
            {
                goto nextColumn;
            }

            tile = map->tiles[(y * map->width) + x];
            // tiled export increments by 1 so we minus 1
            atlasLookup = (tile & 0xFFFFFFF) - 1;

            if (tile & 0x80000000)
            {
                flip = SDL_FLIP_HORIZONTAL;
            }
            else if (tile & 0x40000000)
            {
                flip = SDL_FLIP_VERTICAL;
            }
            else if (tile & 0x20000000)
            {
                // diagonal
                flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else
            {
                flip = SDL_FLIP_NONE;
            }

            src = {(atlasLookup % tileColumns) * map->tileSize,
                   (atlasLookup / tileColumns) * map->tileSize, map->tileSize, map->tileSize};

            SDL_RenderCopyEx(app->renderer, map->atlas.texture, &src, &dest, 0, NULL, flip);
        nextColumn:;
        }
    nextRow:;
    }
}
