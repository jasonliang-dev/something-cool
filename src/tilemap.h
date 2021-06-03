#pragma once

#include "language.h"
#include "renderer.h"
#include "texture.h"

typedef struct TilemapLayer TilemapLayer;
struct TilemapLayer
{
    Quad *quads;
    i32 quadCount;
};

typedef struct Tilemap Tilemap;
struct Tilemap
{
    Texture atlas;
    TilemapLayer layers[2];
    b8 *collision;

    i32 width; // in tiles
    i32 height;

    i32 tileWidth; // in pixels
    i32 tileHeight;
};

typedef struct TilemapMovement TilemapMovement;
struct TilemapMovement
{
    v2 pos;
    v2 vel;
};

Tilemap CreateTilemap(const char *jsonFile, Texture atlas);
void DrawTilemap(const Tilemap *map);

TilemapMovement MoveWithTilemap(const Tilemap *map, v2 pos, v2 vel, v4 rect);
b32 TilemapVersusV2(const Tilemap *map, v2 pos);
b32 TilemapVersusRect(const Tilemap *map, v2 pos, v4 rect);
