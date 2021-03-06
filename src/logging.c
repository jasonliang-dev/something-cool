#define MAX_LOG_STRING_BUFFER 8192

#define LOG(fd, prefix)                                                                            \
    char buff[32];                                                                                 \
    time_t rawTime;                                                                                \
    time(&rawTime);                                                                                \
    struct tm *timeinfo = localtime(&rawTime);                                                     \
    strftime(buff, 32, "[%b %d %I:%M:%S %p] ", timeinfo);                                          \
    fputs(buff, fd);                                                                           \
                                                                                                   \
    fputs(prefix, fd);                                                                      \
                                                                                                   \
    va_list args;                                                                                  \
    va_start(args, format);                                                                        \
    vfprintf(fd, format, args);                                                                \
    va_end(args);                                                                                  \
                                                                                                   \
    fputs("\n", fd)

typedef struct LoggingData LoggingData;
struct LoggingData
{
    const char *logfileLocation;
    char *stringBuffer;
};

global LoggingData loggingData;

internal void StartLogging(const char *logfile)
{
    loggingData.logfileLocation = logfile;
    loggingData.stringBuffer = malloc(sizeof(char) * MAX_LOG_STRING_BUFFER);
    assert(loggingData.stringBuffer);
}

internal void DestroyLogging()
{
    free(loggingData.stringBuffer);
}

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
