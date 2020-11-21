#include "include.c"

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
    os->DebugPrint("BLUE");
}
