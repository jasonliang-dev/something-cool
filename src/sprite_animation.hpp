#pragma once

#include "language.hpp"
#include <HandmadeMath.h>

struct SpriteAnimation
{
    SpriteAnimation(void) = default;
    SpriteAnimation(v4 rect, i32 frames, i32 msPerFrame = 100) noexcept;

    void Update(f32 deltaTime);
    v4 GetFrameRect(void) const;

    v4 m_Rect;
    i32 m_Frames;
    i32 m_MsPerFrame;
    f32 m_ElapsedTime = 0.0f; // between 0 - (m_MsPerFrame * m_Frames)
};
