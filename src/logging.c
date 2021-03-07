#define MAX_LOG_STRING_BUFFER 8192

global const char *logfileLocation = NULL;
global char *logStringBuffer = NULL;

internal inline void StartLogging(const char *logfile)
{
    logfileLocation = logfile;
    logStringBuffer = malloc(MAX_LOG_STRING_BUFFER);
    assert(logStringBuffer);
}

internal void Log(FILE *fd, const char *prefix, const char *format, va_list args)
{
    FILE *logfile = fopen(logfileLocation, "a");
    assert(logfile);

    char buff[32];

    time_t rawTime;
    time(&rawTime);
    struct tm *timeinfo = localtime(&rawTime);
    strftime(buff, 32, "[%b %d %I:%M:%S %p] ", timeinfo);

    vsnprintf(logStringBuffer, MAX_LOG_STRING_BUFFER, format, args);

    fputs(buff, fd);
    fputs(prefix, fd);
    fputs(logStringBuffer, fd);
    fputs("\n", fd);

    fputs(buff, logfile);
    fputs(prefix, logfile);
    fputs(logStringBuffer, logfile);
    fputs("\n", logfile);

    fclose(logfile);
}

internal inline void LogInfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stdout, "[INFO]: ", format, args);
    va_end(args);
}

internal inline void LogWarn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, "[WARN]: ", format, args);
    va_end(args);
}

internal inline void LogError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, "[ERROR]: ", format, args);
    va_end(args);
}

internal inline void LogFatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, "[FATAL]: ", format, args);
    va_end(args);

    exit(1);
}

internal inline void OutOfMemory()
{
    LogFatal("Out of memory");
}
