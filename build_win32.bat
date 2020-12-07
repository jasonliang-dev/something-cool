@echo off

set build_options= -DDEBUG=1 -DPLATFORM_WIN32=1 -DWIN32_LEAN_AND_MEAN=1
set compile_flags= -nologo -W4 -wd4201 -wd4996 -Z7 /I ../src/
set link_flags= -opt:ref -incremental:no -Debug:fastlink
set library_link= gdi32.lib user32.lib winmm.lib opengl32.lib

if not exist build mkdir build
pushd build

cl %build_options% %compile_flags% ../src/win32_main.c^
    /link %library_link% %link_flags% /out:win32_app.exe

popd
