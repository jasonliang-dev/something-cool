@echo off

set BUILD_OPTS= -DDEBUG -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS

set COMPILE_FLAGS= -nologo -W4 -wd4201 -wd4459 -Z7
REM 4201: unnamed struct/union (definition of v2, v3, v4)
REM 4459: declaration of `foo` hides global (thanks imgui)

set INCLUDE_DIRS=^
    /I ../third^
    /I ../third/gl3w^
    /I ../third/imgui^
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

if not exist SDL2.dll copy ..\third\sdl2\lib\x64\SDL2.dll

cl %COMPILE_FLAGS% -D_CRT_SECURE_NO_WARNINGS ../src/shadergen.cpp || exit /b
shadergen.exe || exit /b

cl %BUILD_OPTS% %COMPILE_FLAGS% %INCLUDE_DIRS% ../src/app.cpp^
    /link %LIBRARY_LINK% %LIBRARY_PATHS% %LINK_FLAGS% /out:app.exe || exit /b

popd
