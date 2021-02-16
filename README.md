## Building for Windows

Download development libraries:

- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL_image](https://www.libsdl.org/projects/SDL_image/)
- [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/)

For each development library

1. Extract each download to a folder:

   - SDL2 -> `./sdl2`
   - SDL2_image -> `./sdl2_image`
   - SDL2_ttf -> `./sdl2_ttf`

   Each folder should have an `include` and `lib` directory

1. Make a `./build` directory. Move `.dll`s from each `lib` directory to `./build`

1. Run `build_win.bat` in this directory

## Building for Linux

1. Download `sdl2`, `sdl2_image`, and `sdl2_ttf` from your package manager

1. Run `build_linux.sh` in this directory
