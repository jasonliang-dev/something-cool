#include "tilemap.hpp"
#include <cassert>
#include <glm/ext/matrix_transform.hpp>
#include <gsl/span>
#include <iostream>
#include <range/v3/algorithm/copy.hpp>

Tilemap::Tilemap(const char *jsonFile, std::shared_ptr<Texture> atlas)
{
    cute_tiled_map_t *map = cute_tiled_load_map_from_file(jsonFile, nullptr);

    // should only have one tileset
    assert(map->tilesets);
    assert(map->tilesets->next == nullptr);

    m_Atlas = atlas;

    m_Width = map->width;
    m_Height = map->height;

    m_TileWidth = map->tilewidth;
    m_TileHeight = map->tileheight;

    for (cute_tiled_layer_t *layer = map->layers; layer != nullptr; layer = layer->next)
    {
        if (strcmp(layer->type.ptr, "tilelayer") == 0)
        {
            m_Layers.push_back(InitializeTiles(layer, map->tilesets->firstgid));
        }
    }

    m_Layers.shrink_to_fit();
    cute_tiled_free_map(map);
}

void Tilemap::Draw(Renderer &renderer, m4 mvp)
{
    renderer.BeginDraw(m_Atlas, mvp);

    for (const auto &tiles : m_Layers)
    {
        ranges::copy(tiles, renderer.AllocateQuads((i32)tiles.size()).begin());
    }

    renderer.EndDraw();
}

std::vector<Renderer::Quad> Tilemap::InitializeTiles(cute_tiled_layer_t *layer,
                                                     i32 firstgid)
{
    std::vector<Renderer::Quad> tiles;
    tiles.reserve(layer->data_count);

    i32 tileColumns = m_Atlas->GetWidth() / m_TileWidth;

    for (i32 y = 0; y < m_Height; ++y)
    {
        for (i32 x = 0; x < m_Width; ++x)
        {
            i32 index = layer->data[(y * m_Width) + x];

            if (index == 0)
            {
                continue;
            }

            m4 transform = glm::translate(
                m4(1), v3((f32)(x * m_TileWidth) + (m_TileWidth / 2.0f),
                          (f32)(y * m_TileHeight) + (m_TileHeight / 2.0f), 0.0f));

            if (index & CUTE_TILED_FLIPPED_HORIZONTALLY_FLAG)
            {
                transform = glm::scale(transform, v3(-1.0f, 1.0f, 1.0f));
            }

            if (index & CUTE_TILED_FLIPPED_VERTICALLY_FLAG)
            {
                transform = glm::scale(transform, v3(1.0f, -1.0f, 1.0f));
            }

            if (index & CUTE_TILED_FLIPPED_DIAGONALLY_FLAG)
            {
                transform = glm::scale(transform, v3(-1.0f, -1.0f, 1.0f));
            }

            transform = glm::translate(
                transform, v3(-m_TileWidth / 2.0f, -m_TileHeight / 2.0f, 0.0f));
            transform =
                glm::scale(transform, v3((f32)m_TileWidth, (f32)m_TileHeight, 1.0f));

            i32 id = cute_tiled_unset_flags(index) - firstgid;

            v2 atlasPos = v2((f32)(id % tileColumns), (f32)(id / tileColumns));

            v4 texCoords = {
                atlasPos.x * m_TileWidth / (f32)m_Atlas->GetWidth(),
                atlasPos.y * m_TileHeight / (f32)m_Atlas->GetHeight(),
                (atlasPos.x + 1.0f) * m_TileWidth / (f32)m_Atlas->GetWidth(),
                (atlasPos.y + 1.0f) * m_TileHeight / (f32)m_Atlas->GetHeight(),
            };

            tiles.push_back({transform, texCoords});
        }
    }

    tiles.shrink_to_fit();
    return tiles;
}
