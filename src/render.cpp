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
    SDL_Rect rect = {x - static_cast<i32>(app->camera.x), y - static_cast<i32>(app->camera.y),
                     s->width * app->scale, s->height * app->scale};
    SDL_RenderCopy(app->renderer, s->texture, NULL, &rect);
}

internal inline v2 Texture_ToV2(Texture *texture)
{
    return {static_cast<f32>(texture->width), static_cast<f32>(texture->height)};
}

internal void Tilemap_Load(Tilemap *map, const char *imagePath, const char *tileMapPath)
{
    map->atlas = Texture_Load(imagePath);

    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(tileMapPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    assert(!layer->next);

    map->width = layer->width;
    map->height = layer->height;
    map->tileSize = tiledMap->tilewidth;
    assert(layer->data_count == map->width * map->height);

    map->indices = static_cast<i32 *>(malloc(sizeof(i32) * layer->data_count));
    assert(map->indices);
    memcpy(map->indices, layer->data, sizeof(i32) * layer->data_count);

    {
        cute_tiled_tileset_t *tileset = tiledMap->tilesets;
        assert(!tileset->next);

        u8 *collisionAtlas = static_cast<u8 *>(malloc(sizeof(u8) * tileset->tilecount));
        assert(collisionAtlas);
        for (cute_tiled_tile_descriptor_t *t = tileset->tiles; t != nullptr; t = t->next)
        {
            collisionAtlas[t->tile_index] = !!t->properties->data.boolean;
        }

        map->collision = static_cast<u8 *>(malloc(sizeof(u8) * layer->data_count));
        assert(map->collision);
        for (i32 y = 0; y < map->height; ++y)
        {
            for (i32 x = 0; x < map->width; ++x)
            {
                i32 tile = (map->indices[(y * map->width) + x] & 0xFFFFFFF) - 1;
                map->collision[(y * map->width) + x] = collisionAtlas[tile];
            }
        }

        free(collisionAtlas);
    }

    cute_tiled_free_map(tiledMap);
}

internal void Tilemap_Free(Tilemap *map)
{
    SDL_DestroyTexture(map->atlas.texture);
    free(map->collision);
    free(map->indices);
}

internal void Tilemap_Draw(Tilemap *map, i32 xOffset, i32 yOffset)
{
    const i32 translate = map->tileSize * app->scale;
    const i32 tileColumns = map->atlas.width / map->tileSize;

    if (app->debug)
    {
        SDL_SetRenderDrawColor(app->renderer, 0xFF, 0x00, 0x00, 0xFF);
    }

    for (i32 y = 0; y < map->height; ++y)
    {
        for (i32 x = 0; x < map->width; ++x)
        {
            SDL_Rect dest = {x * translate + xOffset - static_cast<i32>(app->camera.x),
                             y * translate + yOffset - static_cast<i32>(app->camera.y), translate,
                             translate};

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
            // indexd export increments by 1 so we minus 1
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
                flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else
            {
                flip = SDL_FLIP_NONE;
            }

            SDL_Rect src = {(tile % tileColumns) * map->tileSize,
                            (tile / tileColumns) * map->tileSize, map->tileSize, map->tileSize};

            SDL_RenderCopyEx(app->renderer, map->atlas.texture, &src, &dest, 0, NULL, flip);

            if (app->debug && map->collision[(y * map->width) + x])
            {
                SDL_RenderDrawRect(app->renderer, &dest);
            }
        }
    }
}
