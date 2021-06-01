#include "sprite_animation.h"
#include "texture.h"
#include <math.h>
#include <string.h>

SpriteAnimation CreateSpriteAnimation(i32 name)
{
    SpriteAnimationEntry entry = ANIMATION_TABLE[name];

    SpriteAnimation result;

    result.rect = entry.rect;
    result.frames = entry.frames;
    result.msPerFrame = 100;
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

void DrawSpriteAnimationExt(const SpriteAnimation *animation, v2 pos, v2 scale, v4 color)
{
    v4 frameRect = animation->rect;
    i32 frame = (i32)(animation->elapsedTime / animation->msPerFrame);
    frameRect.x += frameRect.z * frame;

    DrawTextureRectExt(pos, frameRect, scale, color);
}
