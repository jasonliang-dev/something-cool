typedef struct
{
    app_permanent_load_fn *PermanentLoad;
    app_hot_load_fn *HotLoad;
    app_hot_unload_fn *HotUnload;
    app_update_fn *Update;
    HMODULE dll;
    FILETIME lastDllWriteTime;
} w32_app_code;

internal FILETIME W32_GetLastWriteTime(char *filename)
{
    FILETIME last_write_time = {0};
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFileA(filename, &find_data);

    if (find_handle != INVALID_HANDLE_VALUE)
    {
        FindClose(find_handle);
        last_write_time = find_data.ftLastWriteTime;
    }

    return last_write_time;
}

internal b32 W32_AppCodeLoad(w32_app_code *appCode, char *dllPath, char *tempDllPath)
{
    appCode->lastDllWriteTime = W32_GetLastWriteTime(dllPath);
    CopyFile(dllPath, tempDllPath, FALSE);
    appCode->dll = LoadLibraryA(tempDllPath);

    if (!appCode->dll)
    {
        return 0;
    }

    appCode->PermanentLoad = (app_permanent_load_fn *)GetProcAddress(appCode->dll, "PermanentLoad");
    appCode->HotLoad = (app_hot_load_fn *)GetProcAddress(appCode->dll, "HotLoad");
    appCode->HotUnload = (app_hot_unload_fn *)GetProcAddress(appCode->dll, "HotUnload");
    appCode->Update = (app_update_fn *)GetProcAddress(appCode->dll, "Update");

    if (!appCode->PermanentLoad || !appCode->HotLoad || !appCode->HotUnload || !appCode->Update)
    {
        appCode->PermanentLoad = AppPermanentLoadStub;
        appCode->HotLoad = AppHotLoadStub;
        appCode->HotUnload = AppHotUnloadStub;
        appCode->Update = AppUpdateStub;
        return 0;
    }

    return 1;
}

internal void W32_AppCodeUnload(w32_app_code *appCode)
{
    if (appCode->dll)
    {
        FreeLibrary(appCode->dll);
    }

    appCode->dll = 0;
    appCode->PermanentLoad = AppPermanentLoadStub;
    appCode->HotLoad = AppHotLoadStub;
    appCode->HotUnload = AppHotUnloadStub;
    appCode->Update = AppUpdateStub;
}

internal void W32_AppCodeHotUpdate(w32_app_code *appCode, char *dllPath, char *tempDllPath)
{
    FILETIME lastWriteTime = W32_GetLastWriteTime(dllPath);

    if (CompareFileTime(&lastWriteTime, &appCode->lastDllWriteTime))
    {
        appCode->HotUnload();
        W32_AppCodeUnload(appCode);
        W32_AppCodeLoad(appCode, dllPath, tempDllPath);
        appCode->HotLoad(&globalOS);
    }
}
