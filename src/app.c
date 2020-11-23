#include "app.h"

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("PERMANENT_LOAD\n");
}

APP_HOT_LOAD
{
    os = os_;
    os->DebugPrint("HOT_LOAD\n");
}

APP_HOT_UNLOAD
{
    os->DebugPrint("HOT_UNLOAD\n");
}

APP_UPDATE
{
    local_persist f32 t = 0;
    f32 bri = Sin(t += 0.1f) + 0.5f;
    glClearColor(bri, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    os->OpenGLSwapBuffers();

    local_persist f32 tSine = 0;

    i32 toneHz = 262; // about middle c (261.63 hz)
    i32 toneVolume = 0;
    i32 wavePeriod = os->samplesPerSecond / toneHz;

    i16 *output = os->sampleOut;
    for (u32 i = 0; i < os->sampleCount; i++)
    {
        i16 value = (i16)(Sin((f32)tSine) * toneVolume);
        *output++ = value;
        *output++ = value;
        tSine += 2.0f * PI * (1.f / (f32)wavePeriod);
    }
}
