internal void W32_ReadFile(memory_arena *arena, char *path, void **data, u64 *len)
{
    *data = 0;
    *len = 0;

    HANDLE file = {0};

    DWORD desired_access = GENERIC_READ | GENERIC_WRITE;
    DWORD share_mode = 0;
    SECURITY_ATTRIBUTES security_attributes = {
        (DWORD)sizeof(SECURITY_ATTRIBUTES),
        0,
        0,
    };
    DWORD creation_disposition = OPEN_EXISTING;
    DWORD flags_and_attributes = 0;
    HANDLE template_file = 0;

    file = CreateFile(path, desired_access, share_mode, &security_attributes, creation_disposition,
                      flags_and_attributes, template_file);
    if (file == INVALID_HANDLE_VALUE)
    {
        return;
    }

    DWORD read_bytes = GetFileSize(file, 0);
    if (!read_bytes)
    {
        goto cleanup;
    }

    void *read_data = MemoryArenaPush(arena, read_bytes + 1);
    DWORD bytes_read = 0;
    OVERLAPPED overlapped = {0};

    ReadFile(file, read_data, read_bytes, &bytes_read, &overlapped);

    ((u8 *)read_data)[read_bytes] = 0;

    *data = read_data;
    *len = (u64)bytes_read;

cleanup:
    CloseHandle(file);
}
