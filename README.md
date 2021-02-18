## Building for Windows

Create a directory, `third`, then download third party development
libraries:

- [SDL2](https://www.libsdl.org/download-2.0.php)

For each development library

1. Extract each download to a new folder:

   - SDL2 -> `./third/sdl2`

   Each folder should have an `include` and `lib` directory

1. Make a `./build` directory. Move `.dll`s from each `lib` directory
   to `./build`.

1. Run `build_win.bat` in this directory

## Building for Linux

1. Download `sdl2`, from your package manager

1. Run `build_linux.sh` in this directory
