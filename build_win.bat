@echo off

if not exist build mkdir build
pushd build

if not exist SDL2.dll copy ..\third\sdl2\lib\x64\SDL2.dll

cl -nologo -W4 -wd4201 -D_CRT_SECURE_NO_WARNINGS ../src/shadergen.c || exit /b
shadergen.exe || exit /b

REM 4201: unnamed struct/union (definition of v2, v3, v4)
set COMPILE_FLAGS=^
    -O2 -DDEBUG -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS^
    -nologo -W4 -wd4201 -Z7^
    /I ../third^
    /I ../third/glad^
    /I ../third/nuklear^
    /I ../third/sdl2/include

set LINK_FLAGS=^
    /link shell32.lib opengl32.lib SDL2.lib SDL2main.lib^
    /LIBPATH:../third/sdl2/lib/x64^
    /SUBSYSTEM:WINDOWS -opt:ref -incremental:no -Debug:fastlink

cl %COMPILE_FLAGS% ../src/app.c %LINK_FLAGS% /out:app.exe || exit /b

popd
