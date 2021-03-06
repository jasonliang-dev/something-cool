global const char *logfileLocation = NULL;

internal void StartLogging(const char *logfile)
{
    logfileLocation = logfile;
}

#define LOG(fd, prefix)                                                                            \
    FILE *logfile = fopen(logfileLocation, "a");                                                   \
    assert(logfile);                                                                               \
                                                                                                   \
    char buff[32];                                                                                 \
    time_t rawTime;                                                                                \
    time(&rawTime);                                                                                \
    struct tm *timeinfo = localtime(&rawTime);                                                     \
    strftime(buff, 32, "[%b %d %I:%M:%S %p] ", timeinfo);                                          \
    fputs(buff, fd);                                                                               \
    fputs(buff, logfile);                                                                          \
                                                                                                   \
    fputs(prefix, fd);                                                                             \
    fputs(prefix, logfile);                                                                        \
                                                                                                   \
    va_list args;                                                                                  \
    va_start(args, format);                                                                        \
    vfprintf(fd, format, args);                                                                    \
    vfprintf(logfile, format, args);                                                               \
    va_end(args);                                                                                  \
                                                                                                   \
    fputs("\n", fd);                                                                               \
    fputs("\n", logfile);                                                                          \
    fclose(logfile)

internal void LogInfo(const char *format, ...)
{
    LOG(stdout, "[INFO]: ");
}

internal void LogWarn(const char *format, ...)
{
    LOG(stderr, "[WARN]: ");
}

internal void LogError(const char *format, ...)
{
    LOG(stderr, "[ERROR]: ");
}

internal void LogFatal(const char *format, ...)
{
    LOG(stderr, "[FATAL]: ");
    exit(1);
}

internal void OutOfMemory()
{
    LogFatal("Out of memory");
}
