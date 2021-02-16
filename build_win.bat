@echo off

set COMPILE_FLAGS= -D_CRT_SECURE_NO_WARNINGS -nologo -W4 -wd4201 -Z7

set INCLUDE_DIRS=^
    /I ../sdl2/include^
    /I ../sdl2_image/include^
    /I ../sdl2_ttf/include

set LIBRARY_LINK=^
    SDL2.lib^
    SDL2main.lib^
    SDL2_image.lib^
    SDL2_ttf.lib

set LIBRARY_PATHS=^
    /LIBPATH:../sdl2/lib/x64^
    /LIBPATH:../sdl2_image/lib/x64^
    /LIBPATH:../sdl2_ttf/lib/x64

set LINK_FLAGS= -opt:ref -incremental:no -Debug:fastlink

if not exist build mkdir build
pushd build

cl %COMPILE_FLAGS% %INCLUDE_DIRS% ../src/main.cpp^
    /link %LIBRARY_LINK% %LIBRARY_PATHS% %LINK_FLAGS% /out:app.exe

popd
