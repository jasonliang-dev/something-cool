#pragma once

#include "language.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include <cute_tiled.h>
#include <memory>
#include <vector>

class Tilemap
{
  public:
    Tilemap(const char *jsonFile, std::shared_ptr<Texture> atlas);

    void Draw(Renderer &renderer, m4 mvp);

  private:
    std::shared_ptr<Texture> m_Atlas;

    i32 m_Width; // in tiles
    i32 m_Height;

    i32 m_TileWidth; // in pixels
    i32 m_TileHeight;

    std::vector<std::vector<Renderer::Quad>> m_Layers;

    std::vector<Renderer::Quad> InitializeTiles(cute_tiled_layer_t *layer, i32 firstgid);
};
