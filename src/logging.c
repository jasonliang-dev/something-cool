global const char *logfileLocation = NULL;

internal inline void StartLogging(const char *logfile)
{
    logfileLocation = logfile;
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

    fputs(buff, fd);
    fputs(buff, logfile);

    fputs(prefix, fd);
    fputs(prefix, logfile);

    vfprintf(fd, format, args);
    vfprintf(logfile, format, args);

    fputs("\n", fd);
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
