typedef struct w32_app_code w32_app_code;
struct w32_app_code
{
    app_permanent_load_fn *PermanentLoad;
    app_hot_load_fn *HotLoad;
    app_hot_unload_fn *HotUnload;
    app_update_fn *Update;
    app_close_fn *Close;
    HMODULE dll;
    FILETIME lastDllWriteTime;
    b32 isValid;
};

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
    Assert(!appCode->isValid);

    WIN32_FILE_ATTRIBUTE_DATA ignored;

    if (!GetFileAttributesEx("lock.tmp", GetFileExInfoStandard, &ignored))
    {
        appCode->lastDllWriteTime = W32_GetLastWriteTime(dllPath);
        CopyFile(dllPath, tempDllPath, FALSE);
        appCode->dll = LoadLibraryA(tempDllPath);

        if (appCode->dll)
        {
            appCode->PermanentLoad =
                (app_permanent_load_fn *)GetProcAddress(appCode->dll, "AppPermanentLoad");
            appCode->HotLoad = (app_hot_load_fn *)GetProcAddress(appCode->dll, "AppHotLoad");
            appCode->HotUnload = (app_hot_unload_fn *)GetProcAddress(appCode->dll, "AppHotUnload");
            appCode->Update = (app_update_fn *)GetProcAddress(appCode->dll, "AppUpdate");
            appCode->Close = (app_update_fn *)GetProcAddress(appCode->dll, "AppClose");

            appCode->isValid = appCode->PermanentLoad && appCode->HotLoad && appCode->HotUnload &&
                               appCode->Update && appCode->Close;
        }
    }

    if (!appCode->isValid)
    {
        appCode->PermanentLoad = AppPermanentLoadStub;
        appCode->HotLoad = AppHotLoadStub;
        appCode->HotUnload = AppHotUnloadStub;
        appCode->Update = AppUpdateStub;
        appCode->Close = AppCloseStub;
        return 0;
    }

    return 1;
}

internal void W32_AppCodeUnload(w32_app_code *appCode)
{
    if (appCode->dll)
    {
        FreeLibrary(appCode->dll);
        appCode->dll = NULL;
    }

    appCode->isValid = 0;
    appCode->PermanentLoad = AppPermanentLoadStub;
    appCode->HotLoad = AppHotLoadStub;
    appCode->HotUnload = AppHotUnloadStub;
    appCode->Update = AppUpdateStub;
    appCode->Close = AppCloseStub;
}

internal void W32_AppCodeMaybeHotLoad(w32_app_code *appCode, char *dllPath, char *tempDllPath)
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
