@echo off

if not exist build mkdir build
pushd build

cl /DWIN32_LEAN_AND_MEAN /nologo -Z7 ../src/run_tests.c^
    /link shell32.lib^
    /opt:ref /incremental:no /debug:fastlink || popd && exit /b 1
run_tests.exe || popd && exit /b 1

popd
