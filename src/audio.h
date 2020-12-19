#define AUDIO_SOURCE_MAX 512

typedef struct sound_t sound_t;
struct sound_t
{
    i32 channels;
    i32 sampleRate;
    u32 sampleCount;
    i16 *samples;
};

typedef struct audio_source_t audio_source_t;
struct audio_source_t
{
    sound_t *sound;
    u32 playPosition;
};

typedef struct audio_t audio_t;
struct audio_t
{
    audio_source_t sources[AUDIO_SOURCE_MAX];
    b8 playing[AUDIO_SOURCE_MAX];
};
