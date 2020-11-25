#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "opengl.h"
#include "os.h"

#include "os.c"
#include "maths.c"
#include "memory.c"

#ifdef _MSC_VER
#define APP_EXPORT __declspec(dllexport)
#else
#define APP_EXPORT
#endif

#define APP_PERMANENT_LOAD APP_EXPORT void PermanentLoad(os_state *os_)
typedef void app_permanent_load_fn(os_state *);
internal void AppPermanentLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_LOAD APP_EXPORT void HotLoad(os_state *os_)
typedef void app_hot_load_fn(os_state *);
internal void AppHotLoadStub(os_state *os_)
{
    (void)os_;
}

#define APP_HOT_UNLOAD APP_EXPORT void HotUnload(void)
typedef void app_hot_unload_fn(void);
internal void AppHotUnloadStub(void)
{
}

#define APP_UPDATE APP_EXPORT void Update(void)
typedef void app_update_fn(void);
internal void AppUpdateStub(void)
{
}
