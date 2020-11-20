@echo off

set build_options= -DDEBUG=1
set compile_flags= -nologo -W4 -wd4201 -Zi

if not exist build mkdir build
pushd build
cl %build_options% %compile_flags% ../src/w32_main.c user32.lib
popd
