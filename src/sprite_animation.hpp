#pragma once

#include "language.hpp"
#include "geometry.hpp"

struct SpriteAnimation
{
    v4 m_Rect;
    i32 m_Frames;
    i32 m_MsPerFrame;
    f32 m_ElapsedTime = 0.0f;

    SpriteAnimation(void) = default;
    SpriteAnimation(v4 rect, i32 frames, i32 msPerFrame = 120);

    void Update(f32 deltaTime);
    v4 GetFrameRect(void) const;
};
