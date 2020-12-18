internal sound_t Sound_LoadFromFile(char *file)
{
    sound_t sound;
    i32 len = stb_vorbis_decode_filename(file, &sound.channels, &sound.sampleRate, &sound.samples);
    Assert(len >= 0);
    sound.sampleCount = (u32)len;
    return sound;
}

internal void Sound_FreeSamples(sound_t *sound)
{
    free(sound->samples);
    sound->samples = NULL;
    sound->sampleCount = 0;
}

internal void Audio_Init(audio_t *audio)
{
    MemorySet(audio->sources, 0, sizeof(audio->sources));
    MemorySet(audio->reserved, 0, sizeof(audio->reserved));
    audio->audioSourceCount = 0;
}

internal void Audio_PlaySound(audio_t *audio, sound_t *sound)
{
    for (u32 i = 0; i < AUDIO_SOURCE_MAX; i++)
    {
        audio_source_t *source = &audio->sources[i];

        if (!source->playing && !audio->reserved[i])
        {
            source->sound = sound;
            source->playing = 1;
            source->playPosition = 0;
            return;
        }
    }
}

internal void Audio_Update(audio_t *audio)
{
    for (u32 i = 0; i < AUDIO_SOURCE_MAX; i++)
    {
        audio_source_t *source = &audio->sources[i];

        if (!source->playing)
        {
            continue;
        }

        i16 *sampleOut = os->sampleOut;
        i16 *sampleLeft = source->sound->samples + source->playPosition;
        i16 *sampleRight = sampleLeft + (source->sound->channels == 2);

        u32 delta = source->sound->sampleCount - source->playPosition;
        u32 targetSampleCount = Min(os->sampleCount, delta);

        for (u32 write = 0; write < targetSampleCount; write++)
        {
            *sampleOut++ += *sampleLeft++;
            *sampleOut++ += *sampleRight++;
            source->playPosition++;

            if (source->playPosition == source->sound->sampleCount)
            {
                source->playing = 0;
                audio->reserved[i] = 0;
                goto out;
            }
        }
    out:;
    }
}
