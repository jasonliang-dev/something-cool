internal void Tilemap_Load(Tilemap *map, const char *imagePath, const char *tileMapPath)
{
    map->atlas = Texture_LoadFromImage(imagePath);

    cute_tiled_map_t *tiledMap = cute_tiled_load_map_from_file(tileMapPath, 0);

    cute_tiled_layer_t *layer = tiledMap->layers;
    assert(!layer->next);

    map->width = layer->width;
    map->height = layer->height;
    map->tileWidth = tiledMap->tilewidth;
    assert(layer->data_count == map->width * map->height);

    map->indices = (i32 *)malloc(sizeof(i32) * layer->data_count);
    assert(map->indices);
    memcpy(map->indices, layer->data, sizeof(i32) * layer->data_count);

    {
        cute_tiled_tileset_t *tileset = tiledMap->tilesets;
        assert(!tileset->next);

        u8 *collisionAtlas = (u8 *)malloc(sizeof(u8) * tileset->tilecount);
        assert(collisionAtlas);
        for (cute_tiled_tile_descriptor_t *t = tileset->tiles; t != nullptr; t = t->next)
        {
            collisionAtlas[t->tile_index] = !!t->properties->data.boolean;
        }

        map->collision = (u8 *)malloc(sizeof(u8) * layer->data_count);
        assert(map->collision);
        for (i32 y = 0; y < map->height; ++y)
        {
            for (i32 x = 0; x < map->width; ++x)
            {
                i32 index = map->indices[(y * map->width) + x];
                if (index == 0)
                {
                    map->collision[(y * map->width + x)] = 0;
                }
                else
                {
                    i32 tile = (index & 0xFFFFFFF) - 1;
                    map->collision[(y * map->width) + x] = collisionAtlas[tile];
                }
            }
        }

        free(collisionAtlas);
    }

    cute_tiled_free_map(tiledMap);
}

internal inline void Tilemap_Free(Tilemap *map)
{
    SDL_DestroyTexture(map->atlas.texture);
    free(map->collision);
    free(map->indices);
}

internal inline v2 Tilemap_BoundingBox(Tilemap *map)
{
    return {(f32)map->width * map->tileWidth * DRAW_SCALE,
            (f32)map->height * map->tileWidth * DRAW_SCALE};
}

internal bool Tilemap_DetectCollision(Tilemap *map, Entity *e)
{
    i32 left = (i32)(e->position.x / (1.0f * map->tileWidth * DRAW_SCALE));
    i32 right = (i32)((e->position.x + (e->image.width * DRAW_SCALE)) /
                      (1.0f * map->tileWidth * DRAW_SCALE));
    i32 top = (i32)(e->position.y / (1.0f * map->tileWidth * DRAW_SCALE));
    i32 bottom = (i32)((e->position.y + (e->image.height * DRAW_SCALE)) /
                       (1.0f * map->tileWidth * DRAW_SCALE));

    if (left < 0)
    {
        left = 0;
    }
    if (right >= map->width)
    {
        right = map->width - 1;
    }
    if (top < 0)
    {
        top = 0;
    }
    if (bottom >= map->height)
    {
        bottom = map->height - 1;
    }

    for (i32 y = top; y <= bottom; ++y)
    {
        for (i32 x = left; x <= right; ++x)
        {
            if (map->collision[(y * map->width) + x])
            {
                return true;
            }
        }
    }

    return false;
}
