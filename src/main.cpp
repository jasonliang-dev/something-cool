#define CUTE_TILED_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "app.hpp"
#include "language.hpp"
#include <cute_tiled.h>
#include <glad/gl.h>
#include <iostream>
#include <stb_image.h>
#include <stdexcept>

int main(void)
{
    Application app;

    try
    {
        app.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        DEBUGGER();
        return EXIT_FAILURE;
    }
}
