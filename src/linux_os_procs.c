internal u64 OS_GetLastWriteTime(const char *filename)
{
    struct stat info;
    stat(filename, &info);
    return (u64)info.st_mtime;
}

internal b32 OS_CopyFile(const char *source, const char *dest)
{
    int input = input = open(source, O_RDONLY);
    if (input == -1)
    {
        LogWarn("Could not open input file: %s", source);
        return false;
    }

    int output = creat(dest, 0755);
    if (output == -1)
    {
        LogWarn("Could not create output file: %s", dest);
        close(input);
        return false;
    }

    off_t bytesCopied = 0;
    struct stat info;
    fstat(input, &info);
    int result = sendfile(output, input, &bytesCopied, info.st_size);

    close(output);
    close(input);

    LogInfo("Copied %s to %s", source, dest);
    return result != -1;
}
