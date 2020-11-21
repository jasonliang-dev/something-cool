@echo off

set build_options= -DDEBUG=1
set compile_flags= -nologo -W4 -wd4201 -Z7
set common_link_flags= opengl32.lib -opt:ref -incremental:no -Debug:fastlink
set platform_link_flags= user32.lib

if not exist build mkdir build
pushd build

start /b /wait "" "cl"^
    %build_options% %compile_flags% ../src/w32_main.c^
    /link %common_link_flags% %platform_link_flags%^
    /out:w32_app.exe

start /b /wait "" "cl"^
    %build_options% %compile_flags% -LD ../src/app.c^
    /link %common_link_flags%^
    /out:app.dll

popd
