#define MAX_AUDIO_SOURCES 512
#define AUDIO_FORMAT AUDIO_S16LSB

typedef struct Sound Sound;
struct Sound
{
    u32 length;
    u8 *buffer;
    SDL_AudioSpec spec;
};

typedef struct Audio Audio;
struct Audio
{
    SDL_AudioDeviceID device;
    Sound *sources[MAX_AUDIO_SOURCES];
    u32 progress[MAX_AUDIO_SOURCES];
    b8 playing[MAX_AUDIO_SOURCES];
};
