cmake_minimum_required(VERSION 3.0)
project(app)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory(third/enet)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third/glfw)

set(SOURCES
  src/geometry.c
  src/input.c
  src/memory.c
  src/player.c
  src/renderer.c
  src/sprite_animation.c
  src/texture.c
  src/tilemap.c
  src/main.c
)

if(WIN32)
  set(SOURCES ${SOURCES} src/os_win32.c)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

# 4505: unreferenced local function has been removed
# 4201: nameless struct/union
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /Wv:18 /wd4505 /wd4201>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -pedantic>
)

target_include_directories(${PROJECT_NAME} PUBLIC
  third/
)

target_link_libraries(${PROJECT_NAME}
  glfw
  OpenGL::GL
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} winmm.lib ws2_32.lib)
endif()
