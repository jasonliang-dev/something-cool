@echo off

REM 4201: unnamed struct/union (definition of v2, v3, v4)
set COMPILE_FLAGS=^
    -DDEBUG -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS^
    -nologo -W4 -wd4201^
    /I ../third^
    /I ../third/glad^
    /I ../third/nuklear^
    /I ../third/sdl2/include

if not exist build mkdir build
pushd build

if not exist SDL2.dll copy ..\third\sdl2\lib\x64\SDL2.dll

del *.pdb > NUL 2> NUL
echo WAITING FOR PDB > hotlock.tmp

cl -Z7 %COMPILE_FLAGS% ../src/hot.c^
    /link /DLL -opt:ref -incremental:no -Debug:fastlink^
    -PDB:hot_%random%.pdb /out:hot.dll || exit /b 1

del hotlock.tmp

cl %COMPILE_FLAGS% ../src/shadergen.c || exit /b 1
shadergen.exe || exit /b 1

cl -Z7 %COMPILE_FLAGS% ../src/app.c^
    /link shell32.lib opengl32.lib SDL2.lib SDL2main.lib^
    /LIBPATH:../third/sdl2/lib/x64^
    -opt:ref -incremental:no -Debug:fastlink^
    /SUBSYSTEM:CONSOLE /out:app.exe || exit /b 1

popd
