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

    i32 width; // in tiles
    i32 height;

    i32 tileWidth; // in pixels
    i32 tileHeight;
};

Tilemap CreateTilemap(const char *jsonFile, Texture atlas);
void DrawTilemap(const Tilemap *map);
