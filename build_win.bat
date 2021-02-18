@echo off

set COMPILE_FLAGS= -D_CRT_SECURE_NO_WARNINGS -nologo -W4 -wd4201 -wd4459 -Z7
REM 4201: unnamed struct/union (definition of v2, v3, v4)
REM 4459: declaration of app shadows global (imgui_demo)

set INCLUDE_DIRS=^
    /I ../src^
    /I ../third/sdl2/include

set LIBRARY_LINK=^
    shell32.lib^
    opengl32.lib^
    SDL2.lib^
    SDL2main.lib

set LIBRARY_PATHS=^
    /LIBPATH:../third/sdl2/lib/x64

set LINK_FLAGS= /SUBSYSTEM:WINDOWS -opt:ref -incremental:no -Debug:fastlink

if not exist build mkdir build
pushd build

cl %COMPILE_FLAGS% %INCLUDE_DIRS% ../src/main.cpp^
    /link %LIBRARY_LINK% %LIBRARY_PATHS% %LINK_FLAGS% /out:app.exe

popd
