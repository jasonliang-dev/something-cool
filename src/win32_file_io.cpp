internal void OS_ReadFile(memory_arena_t *arena, char *path, void **data, u32 *len)
{
    *data = 0;
    *len = 0;

    HANDLE file = {0};

    DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = 0;
    SECURITY_ATTRIBUTES securityAttributes = {
        (DWORD)sizeof(SECURITY_ATTRIBUTES),
        0,
        0,
    };
    DWORD creationDisposition = OPEN_EXISTING;
    DWORD flagsAndAttributes = 0;
    HANDLE templateFile = 0;

    file = CreateFile(path, desiredAccess, shareMode, &securityAttributes, creationDisposition,
                      flagsAndAttributes, templateFile);
    if (file == INVALID_HANDLE_VALUE)
    {
        return;
    }

    DWORD readBytes = GetFileSize(file, 0);
    if (!readBytes)
    {
        CloseHandle(file);
    }

    u8 *readData = (u8 *)M_ArenaPush(arena, readBytes + 1);
    DWORD bytesRead = 0;
    OVERLAPPED overlapped = {0};

    ReadFile(file, readData, readBytes, &bytesRead, &overlapped);

    readData[readBytes] = 0;

    *data = readData;
    *len = bytesRead;

    CloseHandle(file);
}
