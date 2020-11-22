#include <dsound.h>

typedef struct w32_sound_output w32_sound_output;
struct w32_sound_output
{
    b32 initialized;
    u32 bufferSize;
    u32 samplesPerSecond;
    u32 bytesPerSample;
    IDirectSoundBuffer8 *buffer;
};

#define DIRECT_SOUND_CREATE_8(name)                                                                \
    HRESULT name(LPCGUID lpcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE_8(direct_sound_create_8_fn);
DIRECT_SOUND_CREATE_8(DirectSoundCreate8Stub)
{
    (void)lpcGuidDevice;
    (void)ppDS8;
    (void)pUnkOuter;
    return E_NOTIMPL;
}

global direct_sound_create_8_fn *DirectSoundCreate8Proc = DirectSoundCreate8Stub;

internal void W32_LoadDirectSound(void)
{
    HMODULE dsoundLib = LoadLibraryA("dsound.dll");

    if (dsoundLib == NULL)
    {
        return;
    }

    DirectSoundCreate8Proc =
        (direct_sound_create_8_fn *)GetProcAddress(dsoundLib, "DirectSoundCreate8");
}

internal void W32_InitDirectSound(HWND window, w32_sound_output *output)
{
    IDirectSound8 *dsound = NULL;

    if (!DirectSoundCreate8Proc || !SUCCEEDED(DirectSoundCreate8Proc(NULL, &dsound, NULL)))
    {
        return;
    }

    if (!SUCCEEDED(dsound->lpVtbl->SetCooperativeLevel(dsound, window, DSSCL_PRIORITY)))
    {
        return;
    }

    output->samplesPerSecond = 44100;
    output->bytesPerSample = 2 * 2;
    output->bufferSize = output->samplesPerSecond * output->bytesPerSample;

    IDirectSoundBuffer *primaryBuffer = NULL;
    IDirectSoundBuffer8 *secondaryBuffer = NULL;
    IDirectSoundBuffer *tempBuffer = NULL;

    DSBUFFERDESC bufferDescription;
    bufferDescription.dwSize = sizeof(DSBUFFERDESC);
    bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    bufferDescription.dwBufferBytes = 0;
    bufferDescription.dwReserved = 0;
    bufferDescription.lpwfxFormat = NULL;
    bufferDescription.guid3DAlgorithm = GUID_NULL;

    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = output->samplesPerSecond;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    if (!SUCCEEDED(
            dsound->lpVtbl->CreateSoundBuffer(dsound, &bufferDescription, &primaryBuffer, NULL)))
    {
        return;
    }

    if (!SUCCEEDED(primaryBuffer->lpVtbl->SetFormat(primaryBuffer, &waveFormat)))
    {
        return;
    }

    bufferDescription.dwFlags = DSBCAPS_CTRLVOLUME;
    bufferDescription.dwBufferBytes = output->bufferSize;
    bufferDescription.lpwfxFormat = &waveFormat;

    if (!SUCCEEDED(
            dsound->lpVtbl->CreateSoundBuffer(dsound, &bufferDescription, &tempBuffer, NULL)))
    {
        return;
    }

    tempBuffer->lpVtbl->QueryInterface(tempBuffer, &IID_IDirectSoundBuffer8,
                                       (void **)&secondaryBuffer);
    tempBuffer->lpVtbl->Release(tempBuffer);

    output->buffer = secondaryBuffer;
    output->initialized = 1;
}

internal void W32_ClearSoundBuffer(w32_sound_output *soundOutput)
{
    if (!soundOutput->initialized)
    {
        return;
    }

    void *region1;
    DWORD region1Size;
    void *region2;
    DWORD region2Size;

    if (SUCCEEDED(soundOutput->buffer->lpVtbl->Lock(soundOutput->buffer, 0, soundOutput->bufferSize,
                                                    &region1, &region1Size, &region2, &region2Size,
                                                    0)))
    {
        u8 *byte = region1;

        for (DWORD i = 0; i < region1Size; i++)
        {
            *byte++ = 0;
        }

        byte = region2;

        for (DWORD i = 0; i < region2Size; i++)
        {
            *byte++ = 0;
        }

        soundOutput->buffer->lpVtbl->Unlock(soundOutput->buffer, region1, region1Size, region2,
                                            region2Size);
    }
}

internal void W32_FillSoundBuffer(w32_sound_output *soundOutput, i16 *source, u32 lockOffset,
                                  u32 bytesToLock)
{
    void *region1;
    DWORD region1Size;
    void *region2;
    DWORD region2Size;

    if (SUCCEEDED(soundOutput->buffer->lpVtbl->Lock(soundOutput->buffer, lockOffset, bytesToLock,
                                                    &region1, &region1Size, &region2, &region2Size,
                                                    0)))
    {
        i16 *dest = (i16 *)region1;

        for (DWORD i = 0; i < region1Size / soundOutput->bytesPerSample; i++)
        {
            *dest++ = *source++;
            *dest++ = *source++;
            soundOutput->sampleIndex++;
        }

        dest = (i16 *)region2;
        for (DWORD i = 0; i < region2Size / soundOutput->bytesPerSample; i++)
        {
            *dest++ = *source++;
            *dest++ = *source++;
            soundOutput->sampleIndex++;
        }

        soundOutput->buffer->lpVtbl->Unlock(soundOutput->buffer, region1, region1Size, region2,
                                            region2Size);
    }
}
