#define MAX_LOG_STRING_BUFFER 8192

#define COLOR_RESET "\033[0m"
#define COLOR_FG_RED "\033[31m"
#define COLOR_FG_GREEN "\033[32m"
#define COLOR_FG_YELLOW "\033[33m"

global const char *logfileLocation = NULL;
global char *logStringBuffer = NULL;

internal inline void StartLog(const char *logfile)
{
    logfileLocation = logfile;
    logStringBuffer = malloc(MAX_LOG_STRING_BUFFER);
    assert(logStringBuffer);
}

internal inline void EndLog()
{
    free(logStringBuffer);
}

internal void Log(FILE *fd, const char *color, const char *prefix, const char *format, va_list args)
{
    FILE *logfile = fopen(logfileLocation, "a");
    assert(logfile);

    char timeBuff[64];

    time_t rawTime;
    time(&rawTime);
    struct tm *timeinfo = localtime(&rawTime);
    strftime(timeBuff, 64, "[ %b %d, %I:%M:%S %p ]", timeinfo);

    // apparently, if you call vfprintf() twice with the same va_list,
    // the program crashes on linux (clang). I think it's UB:
    //
    //     vfprintf(fd, "", args);
    //     vfprintf(logfile, "", args);
    //
    // okay fine, I guess we use a big char * buffer instead.

    vsnprintf(logStringBuffer, MAX_LOG_STRING_BUFFER, format, args);

    fprintf(fd, "%s%s %s %s\n" COLOR_RESET, color, timeBuff, prefix, logStringBuffer);
    fprintf(logfile, "%s %s %s\n", timeBuff, prefix, logStringBuffer);

    fclose(logfile);
}

internal inline void LogInfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stdout, "", "[ INFO ]:", format, args);
    va_end(args);
}

internal inline void LogWarn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, COLOR_FG_YELLOW, "[ WARN ]:", format, args);
    va_end(args);
}

internal inline void LogError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, COLOR_FG_RED, "[ ERROR ]:", format, args);
    va_end(args);
}

internal inline void LogFatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    Log(stderr, COLOR_FG_RED, "[ FATAL ]:", format, args);
    va_end(args);

    exit(1);
}

internal inline void OutOfMemory()
{
    LogFatal("Out of memory");
}
