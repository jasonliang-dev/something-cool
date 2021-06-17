#include "sprite_animation.h"
#include "assets.h"
#include "renderer.h"
#include "texture.h"
#include <math.h>
#include <string.h>

SpriteAnimation CreateSpriteAnimation(i32 name, i32 msPerFrame)
{
    SpriteAnimationEntry entry = ANIMATION_TABLE[name];

    SpriteAnimation result;

    result.rect = entry.rect;
    result.frames = entry.frames;
    result.msPerFrame = msPerFrame;
    result.elapsedTime = 0;

    return result;
}

void ResetSpriteAnimation(SpriteAnimation *animation)
{
    animation->elapsedTime = 0;
}

void UpdateSpriteAnimation(SpriteAnimation *animation, f32 deltaTime)
{
    animation->elapsedTime = fmodf(animation->elapsedTime + (deltaTime * 1000.0f),
                                   (f32)(animation->msPerFrame * animation->frames));
}

void DrawSpriteAnimation(const SpriteAnimation *animation, v2 pos)
{
    DrawSpriteAnimationExt(animation, pos, v2(1, 1), v4(1, 1, 1, 1));
}

static void DrawTextureRectExt(Texture texture, v2 pos, v4 rect, v2 scale, v4 color)
{
    m4 transform = M4Translate(m4(1), v3(pos.x - (rect.z * scale.x * 0.5f),
                                         pos.y - (rect.w * scale.y * 0.5f), 0.0f));
    transform = M4Scale(transform, v3(rect.z * scale.x, rect.w * scale.y, 1.0f));

    v4 texCoords = {
        rect.x / texture.width,
        rect.y / texture.height,
        (rect.x + rect.z) / texture.width,
        (rect.y + rect.w) / texture.height,
    };

    DrawQuad(transform, texCoords, texture.id, color);
}

void DrawSpriteAnimationExt(const SpriteAnimation *animation, v2 pos, v2 scale, v4 color)
{
    v4 frameRect = animation->rect;
    i32 frame = (i32)(animation->elapsedTime / animation->msPerFrame);
    frameRect.x += frameRect.z * frame;

    DrawTextureRectExt(tex_Atlas, pos, frameRect, scale, color);
}
