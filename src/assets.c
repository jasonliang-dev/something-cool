#include "assets.h"
#include <string.h>

Font fnt_Primary;
Texture tex_White;
Texture tex_Atlas;
Tilemap map_Test;

static Texture CreateWhiteTexture(void)
{
    enum
    {
        WIDTH = 1,
        HEIGHT = 1,
    };

    Texture result;
    result.width = WIDTH;
    result.height = HEIGHT;

    u8 rgba[4 * WIDTH * HEIGHT];
    memset(rgba, 255, sizeof(rgba));

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rgba);

    return result;
}

void InitPermanentAssets(void)
{
    fnt_Primary = CreateFontFace("data/Kenney Pixel Square.ttf", 32.0f);
    tex_White = CreateWhiteTexture();
    tex_Atlas = CreateTexture("data/atlas.png");
    map_Test = CreateTilemap("data/test.json", tex_Atlas);
}
