struct w32_sound_output_t
{
    IMMDeviceEnumerator *device_enum;
    IMMDevice *device;
    IAudioClient *audioClient;
    IAudioRenderClient *audioRenderClient;
    REFERENCE_TIME soundBufferDuration;
    u32 bufferFrameCount;
    u32 samplesPerSecond;
    u32 latencyFrameCount;
    u16 channels;
};

const GUID IID_IAudioClient = {0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2,
                               0xF5,       0x68,   0xA7,   0x03, 0xB2};
const GUID IID_IAudioRenderClient = {0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD,
                                     0xDC,       0xA7,   0xC2,   0x60, 0xE2};
const GUID CLSID_MMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4,
                                       0x57,       0x92,   0x91,   0x69, 0x2E};
const GUID IID_IMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE,
                                      0x8D,       0xB6,   0x36,   0x17, 0xE6};

#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif

#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
#endif

#define SOUND_LATENCY_FPS 15
#define REFTIMES_PER_SEC 10000000

#define CO_CREATE_INSTANCE(name)                                                                   \
    HRESULT name(REFCLSID rclsid, LPUNKNOWN *pUnkOuter, DWORD dwClsContext, REFIID riid,           \
                 LPVOID *ppv)
typedef CO_CREATE_INSTANCE(co_create_instance_fn);
CO_CREATE_INSTANCE(CoCreateInstanceStub)
{
    (void)rclsid;
    (void)pUnkOuter;
    (void)dwClsContext;
    (void)riid;
    (void)ppv;
    return E_NOTIMPL;
}
global co_create_instance_fn *CoCreateInstanceProc = CoCreateInstanceStub;

#define CO_INITIALIZE_EX(name) HRESULT name(LPVOID pvReserved, DWORD dwCoInit)
typedef CO_INITIALIZE_EX(co_initialize_ex_fn);
CO_INITIALIZE_EX(CoInitializeExStub)
{
    (void)pvReserved;
    (void)dwCoInit;
    return E_NOTIMPL;
}
global co_initialize_ex_fn *CoInitializeExProc = CoInitializeExStub;

internal void W32_LoadWASAPI(void)
{
    CoCreateInstanceProc = CoCreateInstanceStub;
    CoInitializeExProc = CoInitializeExStub;

    HMODULE wasapiLib = LoadLibraryA("ole32.dll");
    if (wasapiLib)
    {
        CoCreateInstanceProc =
            (co_create_instance_fn *)GetProcAddress(wasapiLib, "CoCreateInstance");
        CoInitializeExProc = (co_initialize_ex_fn *)GetProcAddress(wasapiLib, "CoInitializeEx");
    }
}

internal b32 W32_InitWASAPI(w32_sound_output_t *output)
{
    CoInitializeExProc(0, COINIT_SPEED_OVER_MEMORY);

    if (FAILED(CoCreateInstanceProc(CLSID_MMDeviceEnumerator, 0, CLSCTX_ALL,
                                    IID_IMMDeviceEnumerator, (void **)(&output->device_enum))))
    {
        OS_DisplayError("WASAPI Error: Device enumerator retrieval failed.");
        return false;
    }

    if (FAILED(output->device_enum->GetDefaultAudioEndpoint(eRender, eConsole, &output->device)))
    {
        OS_DisplayError("WASAPI Error: Default audio endpoint was not found.");
        return false;
    }

    if (FAILED(output->device->Activate(IID_IAudioClient, CLSCTX_ALL, 0,
                                        (void **)&output->audioClient)))
    {
        OS_DisplayError("WASAPI Error: Could not activate audio device.");
        return false;
    }

    WAVEFORMATEX *waveFormat = 0;

    output->audioClient->GetMixFormat(&waveFormat);

    WORD bitsPerSample = 16;
    WORD blockAlign = (output->channels * bitsPerSample) / 8;

    WAVEFORMATEX newWaveFormat = {
        WAVE_FORMAT_PCM,
        (WORD)output->channels,
        output->samplesPerSecond,
        blockAlign * output->samplesPerSecond,
        blockAlign,
        bitsPerSample,
        0,
    };

    output->latencyFrameCount = output->samplesPerSecond / SOUND_LATENCY_FPS;

    REFERENCE_TIME requestedSoundDuration = REFTIMES_PER_SEC * 2;
    if (FAILED(output->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                               AUDCLNT_STREAMFLAGS_RATEADJUST |
                                                   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                                   AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
                                               requestedSoundDuration, 0, &newWaveFormat, 0)))
    {
        OS_DisplayError("WASAPI Error: Audio client initialization failed.");
        return false;
    }

    if (FAILED(output->audioClient->GetService(IID_IAudioRenderClient,
                                               (void **)&output->audioRenderClient)))
    {
        OS_DisplayError("WASAPI Error: Request for audio render service failed.");
        return false;
    }

    output->audioClient->GetBufferSize(&output->bufferFrameCount);

    output->soundBufferDuration = (REFERENCE_TIME)(
        (f64)REFTIMES_PER_SEC * output->bufferFrameCount / output->samplesPerSecond);

    output->audioClient->Start();

    return true;
}

internal void W32_CleanUpWASAPI(w32_sound_output_t *output)
{
    output->audioClient->Stop();

    output->device_enum->Release();
    output->device->Release();
    output->audioClient->Release();
    output->audioRenderClient->Release();
}

internal void W32_FillSoundBuffer(u32 sampleCount, i16 *samples, w32_sound_output_t *output)
{
    BYTE *data = 0;
    DWORD flags = 0;

    output->audioRenderClient->GetBuffer(sampleCount, &data);

    if (data)
    {
        MemoryCopy(data, samples, sizeof(i16) * sampleCount * output->channels);
    }

    output->audioRenderClient->ReleaseBuffer(sampleCount, flags);
}
