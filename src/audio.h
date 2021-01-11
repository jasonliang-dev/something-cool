#define AUDIO_SOURCE_MAX 512

struct sound_t
{
    i32 channels;
    i32 sampleRate;
    u32 sampleCount;
    i16 *samples;
};

struct audio_source_t
{
    sound_t *sound;
    u32 playPosition;
};

struct audio_t
{
    f32 volume;
    audio_source_t sources[AUDIO_SOURCE_MAX];
    b8 playing[AUDIO_SOURCE_MAX];
};
