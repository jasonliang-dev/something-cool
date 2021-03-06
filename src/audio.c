internal void AudioCallback(void *userdata, u8 *stream, int length)
{
    Audio *audio = userdata;

    memset(stream, 0, length);

    for (i32 i = 0; i < MAX_AUDIO_SOURCES; ++i)
    {
        if (audio->playing[i])
        {
            i32 remaining = audio->sources[i]->length - audio->progress[i];
            i32 write = Min(remaining, length);

            SDL_MixAudioFormat(stream, audio->sources[i]->buffer + audio->progress[i], AUDIO_FORMAT,
                               write, SDL_MIX_MAXVOLUME / 2);
            audio->progress[i] += write;

            if (audio->progress[i] == audio->sources[i]->length)
            {
                audio->playing[i] = false;
            }
        }
    }
}

internal void InitAudio(Audio *audio)
{
    SDL_AudioSpec want;
    want.freq = 44100;
    want.format = AUDIO_FORMAT;
    want.channels = 1;
    want.silence = 0;
    want.samples = 4096;
    want.padding = 0;
    want.size = 0;
    want.callback = AudioCallback;
    want.userdata = &app->audio;

    audio->device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

    memset(audio->playing, 0, sizeof(audio->playing));
}

internal void CreateSoundFromWAV(Sound *sound, const char *filename)
{
    assert(SDL_LoadWAV(filename, &sound->spec, &sound->buffer, &sound->length));
}

internal void PlaySound(Audio *audio, Sound *sound)
{
    for (i32 i = 0; i < MAX_AUDIO_SOURCES; ++i)
    {
        if (!audio->playing[i])
        {
            audio->playing[i] = true;
            audio->sources[i] = sound;
            audio->progress[i] = 0;
            return;
        }
    }
}
