## Building for Windows

Create a directory, `third`, then download third party development libraries:

- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [GLEW](http://glew.sourceforge.net/)

For each development library

1. Extract each download to a new folder:

   - SDL2 -> `./third/sdl2`
   - SDL2_image -> `./third/sdl2_image`
   - SDL2_ttf -> `./third/sdl2_ttf`
   - GLEW -> `./third/glew`

   Each folder should have an `include` and `lib` directory

1. Make a `./build` directory. For the SDL libraries, Move `.dll`s
   from each `lib` directory to `./build`.

1. Run `build_win.bat` in this directory

## Building for Linux

1. Download `sdl2`, `sdl2_image`, and `sdl2_ttf` from your package manager

1. Run `build_linux.sh` in this directory
