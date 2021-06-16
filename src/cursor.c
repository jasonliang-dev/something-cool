#include "cursor.h"
#include "renderer.h"
#include "input.h"
#include "assets.h"

void DrawCursor(v2 scale)
{
    static const v4 s_Rect = {32.0f, 480.0f, 16.0f, 16.0f};

    v2 pos = MousePos();
    pos.x /= scale.x;
    pos.y /= scale.y;

    m4 transform =
        M4Translate(m4(1), v3(pos.x - (s_Rect.z / 2), pos.y - (s_Rect.w / 2), 0.0f));
    transform = M4Scale(transform, v3(s_Rect.z, s_Rect.w, 1.0f));

    v4 texCoords = {
        s_Rect.x / tex_Atlas.width,
        s_Rect.y / tex_Atlas.height,
        (s_Rect.x + s_Rect.z) / tex_Atlas.width,
        (s_Rect.y + s_Rect.w) / tex_Atlas.height,
    };

    DrawQuad(transform, texCoords, tex_Atlas.id, v4(1, 1, 1, 1));
}
