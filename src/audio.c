#define STB_VORBIS_HEADER_ONLY

#include "audio.h"
#include "memory.h"
#include <assert.h>
#include <miniaudio.h>
#include <stb_vorbis.c>
#include <string.h>

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_CHANNEL_COUNT 2
#define AUDIO_SOURCE_MAX 64

typedef struct AudioSource AudioSource;
struct AudioSource
{
    Sound sound;
    i32 cursor;
    b32 playing;
};

static struct
{
    ma_device device;
    AudioSource sources[AUDIO_SOURCE_MAX];
} g_Audio;

static void DataCallback(ma_device *device, void *output, const void *input,
                         ma_uint32 frameCount)
{
    (void)device;
    (void)input;

    for (i32 i = 0; i < AUDIO_SOURCE_MAX; ++i)
    {
        AudioSource *source = g_Audio.sources + i;
        if (!source->playing)
        {
            continue;
        }

        i16 *out = output;
        i16 *sample = source->sound.samples + (source->cursor * AUDIO_CHANNEL_COUNT);

        for (u32 f = 0; f < frameCount; ++f)
        {
            for (i32 c = 0; c < AUDIO_CHANNEL_COUNT; ++c)
            {
                *out++ += *sample++;
            }

            source->cursor++;
            if (source->cursor >= source->sound.sampleCount)
            {
                source->playing = false;
                break;
            }
        }
    }
}

void InitAudio(void)
{
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_s16;
    config.playback.channels = AUDIO_CHANNEL_COUNT;
    config.sampleRate = AUDIO_SAMPLE_RATE;
    config.dataCallback = DataCallback;
    config.pUserData = NULL;

    if (ma_device_init(NULL, &config, &g_Audio.device) != MA_SUCCESS)
    {
        Fatal("Cannot initialize audio device");
    }

    if (ma_device_start(&g_Audio.device) != MA_SUCCESS)
    {
        Fatal("Failed to start audio device");
    }

    memset(g_Audio.sources, 0, sizeof(g_Audio.sources));
}

Sound CreateSound(const char *file)
{
    Sound result;

    i16 *output;
    i32 len =
        stb_vorbis_decode_filename(file, &result.channels, &result.sampleRate, &output);

    if (!output)
    {
        Fatal("Can't load sound file %s", file);
    }

    assert(result.channels == AUDIO_CHANNEL_COUNT);
    assert(result.sampleRate == AUDIO_SAMPLE_RATE);

    result.sampleCount = len;
    result.samples = PermanentAlloc(sizeof(i16) * len * result.channels);
    memcpy(result.samples, output, sizeof(i16) * len * result.channels);
    free(output);

    return result;
}

void StartSound(const Sound sound)
{
    for (i32 i = 0; i < AUDIO_SOURCE_MAX; ++i)
    {
        if (!g_Audio.sources[i].playing)
        {
            g_Audio.sources[i].sound = sound;
            g_Audio.sources[i].cursor = 0;
            g_Audio.sources[i].playing = true;
            return;
        }
    }

    printf("Not playing sound. Too many audio sources.\n");
}
