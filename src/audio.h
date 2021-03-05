#define MAX_AUDIO_SOURCES 512
#define AUDIO_FORMAT AUDIO_S16LSB

typedef struct Sound Sound;
struct Sound
{
    u32 length;
    u8 *buffer;
    SDL_AudioSpec spec;
};

typedef struct AudioSource AudioSource;
struct AudioSource
{
    Sound *sound;
    u32 progress;
};

typedef struct Audio Audio;
struct Audio
{
    SDL_AudioDeviceID device;
    AudioSource sources[MAX_AUDIO_SOURCES];
    b8 playing[MAX_AUDIO_SOURCES];
};
