#include <xaudio2.h>

#define X_AUDIO_2_CREATE(name)                                                                     \
    HRESULT name(IXAudio2 **ppXAudio2, UINT32 flags, XAUDIO2_PROCESSOR XAudio2Processor)
typedef X_AUDIO_2_CREATE(x_audio_2_create_fn);
X_AUDIO_2_CREATE(XAudio2CreateStub)
{
    (void)ppXAudio2;
    (void)flags;
    (void)XAudio2Processor;
    return XAUDIO2_E_INVALID_CALL;
}
global x_audio_2_create_fn *XAudio2CreateProc = XAudio2CreateStub;

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

internal HRESULT W32_XAudio2FindChunk(HANDLE hFile, DWORD fourcc, DWORD *dwChunkSize,
                                      DWORD *dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }

        if (dwChunkType == fourccRIFF)
        {
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
            }
        }
        else if (INVALID_SET_FILE_POINTER ==
                 SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
        {

            return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            *dwChunkSize = dwChunkDataSize;
            *dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize)
        {
            return S_FALSE;
        }
    }

    return S_OK;
}

internal HRESULT W32_XAudio2ReadChunkData(HANDLE hFile, void *buffer, DWORD buffersize,
                                          DWORD bufferoffset)
{
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD dwRead;
    if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

internal void W32_LoadXAudio2(void)
{
    HMODULE xaudio2Lib = LoadLibraryA("xaudio2.dll");

    if (xaudio2Lib)
    {
        XAudio2CreateProc = (x_audio_2_create_fn *)GetProcAddress(xaudio2Lib, "XAudio2Create");
    }
}

internal void W32_InitXAudio2(void)
{
    IXAudio2 *pXAudio2 = NULL;
    if (FAILED(XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
    {
        return;
    }

    IXAudio2MasteringVoice *pMasterVoice = NULL;
    if (FAILED(pXAudio2->lpVtbl->CreateMasteringVoice(pXAudio2,      // this
                                                      &pMasterVoice, // **ppMasteringVoice
                                                      XAUDIO2_DEFAULT_CHANNELS, // InputChannels
                                                      44100,                    // InputSampleRate
                                                      0,                        // Flags
                                                      NULL,                     // szDeviceId
                                                      NULL,                     // *pEffectChain
                                                      AudioCategory_GameMedia   // StreamCategory
                                                      )))
    {
        return;
    }
}
