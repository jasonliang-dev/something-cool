cmake_minimum_required(VERSION 3.0)
project(app)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third/enet)
add_subdirectory(third/glfw)

set(SOURCES
  src/assets.c
  src/audio.c
  src/font.c
  src/geometry.c
  src/input.c
  src/memory.c
  src/net.c
  src/player.c
  src/renderer.c
  src/scene_join_server.c
  src/scene_lobby.c
  src/scene_main_menu.c
  src/scene_options.c
  src/scene_setup_port.c
  src/scene_test.c
  src/scenes.c
  src/sprite_animation.c
  src/texture.c
  src/tilemap.c
  src/ui.c
  src/window.c
  src/main.c
)

if(WIN32)
  set(SOURCES ${SOURCES} src/os_win32.c)
endif()

if(UNIX AND NOT APPLE)
  set(SOURCES ${SOURCES} src/os_linux.c)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

# 4505: unreferenced local function has been removed
# 4201: nameless struct/union
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4 /WX /Wv:18
    /wd4505 /wd4201
  >
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
    -Wall -Wextra -Werror -pedantic
    -Wno-missing-braces -Wno-unused-function
  >
)

target_include_directories(${PROJECT_NAME} PUBLIC
  third/
  third/enet/include/
)

target_link_libraries(${PROJECT_NAME}
  enet
  glfw
  OpenGL::GL
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} winmm.lib ws2_32.lib)
endif()
