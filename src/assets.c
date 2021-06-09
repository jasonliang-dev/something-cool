#include "assets.h"

Font fnt_Primary;
Texture tex_Atlas;
Tilemap map_Test;

void InitPermanentAssets(void)
{
    fnt_Primary = CreateFontFace("data/Kenney Pixel Square.ttf", 32.0f);
    tex_Atlas = CreateTexture("data/atlas.png");
    map_Test = CreateTilemap("data/test.json", tex_Atlas);
}
