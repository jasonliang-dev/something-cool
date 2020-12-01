#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"

#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"
#include "opengl.h"
#include "render.h"

#include "os.c"
#include "maths.c"
#include "memory.c"
#include "opengl.c"
#include "render.c"

typedef struct app_state app_state;
struct app_state {
    sprite_data sprite;
    u32 spriteShader;
};

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
