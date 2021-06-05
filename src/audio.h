#pragma once

#include "language.h"

void InitAudio(void);

typedef struct Sound Sound;
struct Sound
{
    i32 channels;
    i32 sampleRate;
    i16 *samples;
    i32 sampleCount;
};

Sound CreateSound(const char *file);
// StartSound because PlaySound IS TAKEN BY WINDOWS WHYYY
void StartSound(const Sound sound);
