internal u64 OS_GetLastWriteTime(const char *filename)
{
    FILETIME lastWriteTime = {0};
    WIN32_FIND_DATA findData;
    HANDLE findHandle = FindFirstFileA(filename, &findData);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime = findData.ftLastWriteTime;
        FindClose(findHandle);
    }

    ULARGE_INTEGER copy;
    copy.u.LowPart = lastWriteTime.dwLowDateTime;
    copy.u.HighPart = lastWriteTime.dwHighDateTime;

    return (copy.QuadPart / 10000000) - 11644473600LL;
}

internal void OS_CopyFile(const char *source, const char *dest)
{
    CopyFile(source, dest, false);
}
