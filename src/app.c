#include "language_layer.h"
#include "program_options.h"
#include "maths.h"
#include "memory.h"
#include "os.h"

#include "maths.c"
#include "memory.c"

APP_PERMANENT_LOAD
{
    os = os_;
    os->DebugPrint("PERMANENT_LOAD\n");
}

APP_HOT_LOAD
{
    os = os_;
    os->DebugPrint("HOT_LOAD\n");
}

APP_HOT_UNLOAD
{
    os->DebugPrint("HOT_UNLOAD\n");
}

APP_UPDATE
{
}
