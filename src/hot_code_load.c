typedef struct HotCode HotCode;
struct HotCode
{
    u64 modTime;
    void *handle;
    void (*Hello)(void);
};

internal void HotCodeLoad(HotCode *hotCode, const char *objPath, const char *copyPath)
{
    FILE *lock = fopen("hotlock.tmp", "r");
    if (lock)
    {
        fclose(lock);
        return;
    }

    OS_CopyFile(objPath, copyPath);
    hotCode->modTime = OS_GetLastWriteTime(objPath);
    hotCode->handle = SDL_LoadObject(copyPath);
    if (hotCode->handle)
    {
        LogInfo("Loaded hot code object");
    }
    else
    {
        LogFatal("Cannot load hot code object");
    }

    hotCode->Hello = (void (*)(void))SDL_LoadFunction(hotCode->handle, "Hello");
    if (!hotCode->Hello)
    {
        LogFatal("Cannot load hot function: Hello");
    }

    LogInfo("Loaded all hot code procedures");
}

internal void HotCodeUnload(HotCode *hotCode)
{
    SDL_UnloadObject(hotCode->handle);
    hotCode->handle = NULL;
    hotCode->Hello = HelloStub;

    LogInfo("Unloaded hot code");
}

internal void HotCodeMaybeReload(HotCode *hotCode, const char *objPath, const char *copyPath)
{
    u64 modTime = OS_GetLastWriteTime(objPath);

    if (modTime > hotCode->modTime)
    {
        HotCodeUnload(hotCode);
        HotCodeLoad(hotCode, objPath, copyPath);
        LogInfo("Reloaded hot code");
    }
}
