#include <stdio.h> // sprintf_s

#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"
#include "opengl.h"

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"

#ifdef _MSC_VER
#define APP_EXPORT __declspec(dllexport)
#else
#define APP_EXPORT
#endif

#define APP_PERMANENT_LOAD APP_EXPORT void AppPermanentLoad(os_state *os_)
typedef void app_permanent_load_fn(os_state *);
internal void AppPermanentLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_LOAD APP_EXPORT void AppHotLoad(os_state *os_)
typedef void app_hot_load_fn(os_state *);
internal void AppHotLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_UNLOAD APP_EXPORT void AppHotUnload(void)
typedef void app_hot_unload_fn(void);
internal void AppHotUnloadStub(void)
{
}

#define APP_UPDATE APP_EXPORT void AppUpdate(void)
typedef void app_update_fn(void);
internal void AppUpdateStub(void)
{
}

#define APP_CLOSE APP_EXPORT void AppClose(void)
typedef void app_close_fn(void);
internal void AppCloseStub(void)
{
}
