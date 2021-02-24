@echo off

if not exist build mkdir build
pushd build

if not exist SDL2.dll copy ..\third\sdl2\lib\x64\SDL2.dll

cl -nologo -W4 -wd4201 -D_CRT_SECURE_NO_WARNINGS ../src/shadergen.cpp || exit /b
shadergen.exe || exit /b

REM 4201: unnamed struct/union (definition of v2, v3, v4)
REM 4459: declaration of `foo` hides global (thanks imgui)
set COMPILE_FLAGS=^
    -DDEBUG -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS^
    -nologo -W4 -wd4201 -wd4459 -Z7^
    /I ../third^
    /I ../third/glad^
    /I ../third/imgui^
    /I ../third/sdl2/include

set LINK_FLAGS=^
    /link shell32.lib opengl32.lib SDL2.lib SDL2main.lib^
    /LIBPATH:../third/sdl2/lib/x64^
    /SUBSYSTEM:CONSOLE -opt:ref -incremental:no -Debug:fastlink

cl %COMPILE_FLAGS% ../src/app.cpp %LINK_FLAGS% /out:app.exe || exit /b

popd
