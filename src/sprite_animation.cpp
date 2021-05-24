#include "sprite_animation.hpp"
#include <cmath>

SpriteAnimation::SpriteAnimation(v4 rect, i32 frames, i32 msPerFrame) noexcept
    : m_Rect(rect), m_Frames(frames), m_MsPerFrame(msPerFrame)
{
}

void SpriteAnimation::Update(f32 deltaTime)
{
    m_ElapsedTime =
        fmod(m_ElapsedTime + (deltaTime * 1000.0f), (f32)(m_MsPerFrame * m_Frames));
}

v4 SpriteAnimation::GetFrameRect(void) const
{
    v4 result = m_Rect;
    i32 frame = (i32)(m_ElapsedTime / m_MsPerFrame);
    result.X += m_Rect.Z * frame;
    return result;
}
