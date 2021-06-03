#include "tilemap.h"
#include "memory.h"
#include <assert.h>
#include <cute_tiled.h>
#include <string.h>

TilemapLayer InitializeTiles(Tilemap *out, cute_tiled_layer_t *layer, i32 firstgid)
{
    i32 tileColumns = out->atlas.width / out->tileWidth;

    TilemapLayer result;
    result.quadCount = 0;

    for (i32 i = 0; i < layer->data_count; ++i)
    {
        if (layer->data[i])
        {
            result.quadCount++;
        }
    }

    result.quads = PermanentAlloc(sizeof(Quad) * result.quadCount);
    Quad *iter = result.quads;

    for (i32 y = 0; y < out->height; ++y)
    {
        for (i32 x = 0; x < out->width; ++x)
        {
            i32 index = layer->data[(y * out->width) + x];

            if (index == 0)
            {
                continue;
            }

            m4 transform = M4Translate(
                m4(1), v3((f32)(x * out->tileWidth) + (out->tileWidth / 2.0f),
                          (f32)(y * out->tileHeight) + (out->tileHeight / 2.0f), 0.0f));

            if (index & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG)
            {
                transform = M4Scale(transform, v3(-1.0f, 1.0f, 1.0f));
            }

            if (index & CUTE_TILED_FLIPPED_VERTICALLY_FLAG)
            {
                transform = M4Scale(transform, v3(1.0f, -1.0f, 1.0f));
            }

            if (index & CUTE_TILED_FLIPPED_DIAGONALLY_FLAG)
            {
                transform = M4Scale(transform, v3(-1.0f, -1.0f, 1.0f));
            }

            transform = M4Translate(
                transform, v3(-out->tileWidth / 2.0f, -out->tileHeight / 2.0f, 0.0f));
            transform =
                M4Scale(transform, v3((f32)out->tileWidth, (f32)out->tileHeight, 1.0f));

            i32 id = cute_tiled_unset_flags(index) - firstgid;

            v2 atlasPos = v2((f32)(id % tileColumns), (f32)(id / tileColumns));

            v4 texCoords = {
                atlasPos.x * out->tileWidth / (f32)out->atlas.width,
                atlasPos.y * out->tileHeight / (f32)out->atlas.height,
                (atlasPos.x + 1.0f) * out->tileWidth / (f32)out->atlas.width,
                (atlasPos.y + 1.0f) * out->tileHeight / (f32)out->atlas.height,
            };

            *iter++ = CreateQuad(transform, texCoords, v4(1, 1, 1, 1));
        }
    }

    return result;
}

Tilemap CreateTilemap(const char *jsonFile, Texture atlas)
{
    Tilemap result;

    cute_tiled_map_t *map = cute_tiled_load_map_from_file(jsonFile, NULL);

    if (!map)
    {
        Fatal("Could not load tilemap: %s", cute_tiled_error_reason);
    }

    result.atlas = atlas;

    // should only have one tileset
    assert(map->tilesets);
    assert(map->tilesets->next == NULL);

    result.width = map->width;
    result.height = map->height;

    result.tileWidth = map->tilewidth;
    result.tileHeight = map->tileheight;

    i32 i = 0;
    for (cute_tiled_layer_t *layer = map->layers; layer; layer = layer->next)
    {
        if (strcmp(layer->type.ptr, "tilelayer") == 0)
        {
            if (strcmp(layer->name.ptr, "Collision") == 0)
            {
                result.collision = PermanentAlloc(sizeof(b8) * layer->data_count);

                for (i32 i = 0; i < layer->data_count; ++i)
                {
                    result.collision[i] = !!layer->data[i];
                }
            }
            else
            {
                result.layers[i] =
                    InitializeTiles(&result, layer, map->tilesets->firstgid);
                ++i;
            }
        }
    }

    cute_tiled_free_map(map);

    return result;
}

void DrawTilemap(const Tilemap *map)
{
    assert(g_Renderer.currentAtlas.id == map->atlas.id);

    for (u32 i = 0; i < ArrayCount(map->layers); ++i)
    {
        Quad *quads = AllocateQuads(map->layers[i].quadCount);
        memcpy(quads, map->layers[i].quads, sizeof(Quad) * map->layers[i].quadCount);
    }
}

TilemapMovement MoveWithTilemap(const Tilemap *map, v2 pos, v2 vel, v4 rect)
{
    enum
    {
        SUB_STEP = 8,
    };

    const v2 subVel = v2(vel.x / SUB_STEP, vel.y / SUB_STEP);

    v2 oldPosition;
    for (i32 i = 0; i < SUB_STEP; ++i)
    {
        oldPosition = pos;
        pos.y += subVel.y;

        if (TilemapVersusRect(map, pos, rect))
        {
            pos = oldPosition;
            vel.y = 0;
        }

        oldPosition = pos;
        pos.x += subVel.x;

        if (TilemapVersusRect(map, pos, rect))
        {
            pos = oldPosition;
            vel.x = 0;
        }
    }

    return (TilemapMovement){pos, vel};
}

b32 TilemapVersusV2(const Tilemap *map, v2 pos)
{
    i32 x = (i32)(pos.x / map->tileWidth);
    i32 y = (i32)(pos.y / map->tileHeight);

    if (x < 0 || x > map->width || y < 0 || y > map->height)
    {
        return false;
    }

    return map->collision[(y * map->width) + x];
}

b32 TilemapVersusRect(const Tilemap *map, v2 pos, v4 rect)
{
    rect.x += pos.x;
    rect.y += pos.y;

    i32 left = (i32)rect.x / map->tileWidth;
    i32 right = (i32)(rect.x + rect.z) / map->tileWidth;

    i32 top = (i32)rect.y / map->tileHeight;
    i32 bottom = (i32)(rect.y + rect.w) / map->tileHeight;

    left = Max(left, 0);
    right = Min(right, map->width - 1);

    top = Max(top, 0);
    bottom = Min(bottom, map->height - 1);

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
