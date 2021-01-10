internal void Audio_LoadSoundFromFile(char *file, sound_t *sound)
{
    i32 len =
        stb_vorbis_decode_filename(file, &sound->channels, &sound->sampleRate, &sound->samples);
    Assert(len >= 0);
    sound->sampleCount = (u32)len;
}

internal void Audio_FreeSoundSamples(sound_t *sound)
{
    free(sound->samples);
    sound->samples = NULL;
    sound->sampleCount = 0;
}

internal void Audio_Init(audio_t *audio)
{
    audio->volume = 1.0f;
    MemorySet(audio->sources, 0, sizeof(audio->sources));
    MemorySet(audio->playing, 0, sizeof(audio->playing));
}

internal void Audio_PlaySound(audio_t *audio, sound_t *sound)
{
    for (u32 i = 0; i < AUDIO_SOURCE_MAX; i++)
    {
        audio_source_t *source = &audio->sources[i];

        if (!audio->playing[i])
        {
            audio->playing[i] = true;
            source->sound = sound;
            source->playPosition = 0;
            return;
        }
    }
}

internal void Audio_Update(audio_t *audio)
{
    for (u32 i = 0; i < AUDIO_SOURCE_MAX; i++)
    {
        if (!audio->playing[i])
        {
            continue;
        }

        audio_source_t *source = &audio->sources[i];

        i16 *sampleOut = os->sampleOut;
        i16 *sampleIn = source->sound->samples + (source->playPosition * source->sound->channels);

        for (u32 write = 0; write < os->sampleCount; write++)
        {
            *sampleOut++ += (i16)(*sampleIn++ * audio->volume);
            *sampleOut++ += (i16)(*sampleIn++ * audio->volume);

            source->playPosition++;
            if (source->playPosition >= source->sound->sampleCount)
            {
                audio->playing[i] = false;
                goto out;
            }
        }
    out:;
    }
}
