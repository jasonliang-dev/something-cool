## Building for Windows

1. [Download SDL2 development library for Visual
   C++](https://www.libsdl.org/download-2.0.php).

2. Create a directory called `sdl2/`, extract the contents of the
   downloaded zip into `sdl2`. `sdl2/include` and `sdl2/lib/x64` are
   required to compile this project.

3. Make `build/` directory. Move `sdl2/lib/x64/SDL2.dll` to `build/`.

4. Run `build_win.bat`.
