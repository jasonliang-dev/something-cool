@echo off

set OUT_EXE=app

REM 4201 nameless struct/union
set FLAGS=^
    /DDEBUG^
    /DWIN32_LEAN_AND_MEAN^
    /D_CRT_SECURE_NO_WARNINGS^
    /Z7 /MD /nologo /W4 /wd4201

set SOURCES=^
    ../src/camera2d.c^
    ../src/camera3d.c^
    ../src/file_io.c^
    ../src/geometry.c^
    ../src/gfx.c^
    ../src/gl.c^
    ../src/input.c^
    ../src/shader.c^
    ../src/shaders.gen.c^
    ../src/texture.c^
    ../src/win32_opengl.c^
    ../src/win32_window.c^
    ../src/main.c

set LIBS= user32.lib gdi32.lib opengl32.lib

if not exist build mkdir build
pushd build

cl %FLAGS% ..\src\shadergen.c && shadergen.exe

cl %FLAGS% %SOURCES% /link %LIBS%^
    /opt:ref /incremental:no /debug:fastlink^
    /ENTRY:mainCRTStartup /SUBSYSTEM:CONSOLE^
    /out:%OUT_EXE%.exe

popd
