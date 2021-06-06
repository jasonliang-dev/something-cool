cmake_minimum_required(VERSION 3.0)
project(app)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory(third/enet)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third/glfw)

add_library(cimgui STATIC
  third/cimgui/cimgui.cpp
  third/cimgui/imgui/imgui.cpp
  third/cimgui/imgui/imgui_demo.cpp
  third/cimgui/imgui/imgui_draw.cpp
  third/cimgui/imgui/imgui_tables.cpp
  third/cimgui/imgui/imgui_widgets.cpp
  third/cimgui/imgui/backends/imgui_impl_glfw.cpp
  third/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_LIBRARIES glfw)

target_compile_definitions(cimgui PUBLIC
  IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
  IMGUI_IMPL_OPENGL_LOADER_GLAD2
)
if (WIN32)
  target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t__declspec\(dllexport\))
  list(APPEND IMGUI_LIBRARIES imm32)
else()
  target_compile_definitions(cimgui PUBLIC IMGUI_IMPL_API=extern\t\"C\"\t)
endif()

target_include_directories(cimgui PUBLIC
  third/
  third/cimgui
  third/cimgui/imgui
)

target_link_libraries(cimgui ${IMGUI_LIBRARIES})

set(SOURCES
  src/audio.c
  src/geometry.c
  src/input.c
  src/memory.c
  src/net.c
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
  cimgui
  enet
  glfw
  OpenGL::GL
)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} winmm.lib ws2_32.lib)
endif()
