@echo off

set build_options= -DDEBUG=1 -DBUILD_WIN32=1
set compile_flags= -nologo -W4 -wd4201 -Z7
set common_link_flags= opengl32.lib -opt:ref -incremental:no -Debug:fastlink
set platform_link_flags= gdi32.lib user32.lib dsound.lib dxguid.lib xaudio2.lib

if not exist build mkdir build
pushd build

del *.pdb > NUL 2> NUL
echo WAITING FOR PDB > lock.tmp

start /b /wait "" "cl"^
    %build_options% %compile_flags% -LD ../src/app.c^
    /link %common_link_flags%^
    -PDB:app_%random%.pdb^
    /out:app.dll

del lock.tmp

start /b /wait "" "cl"^
    %build_options% %compile_flags% ../src/win32_main.c^
    /link %common_link_flags% %platform_link_flags%^
    /out:win32_app.exe

popd
