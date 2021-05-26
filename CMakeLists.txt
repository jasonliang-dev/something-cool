cmake_minimum_required(VERSION 3.0)
project(app)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory(tests)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory(third/enet)
add_subdirectory(third/entt)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third/glfw)

add_library(imgui
  third/imgui/imgui.cpp
  third/imgui/imgui_demo.cpp
  third/imgui/imgui_draw.cpp
  third/imgui/imgui_tables.cpp
  third/imgui/imgui_widgets.cpp
  third/imgui/backends/imgui_impl_glfw.cpp
  third/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
  third/
  third/imgui/
)

target_link_libraries(imgui PUBLIC glfw)

add_executable(${PROJECT_NAME}
  src/app.cpp
  src/input.cpp
  src/renderer.cpp
  src/sprite_animation.cpp
  src/texture.cpp
  src/tilemap.cpp
  src/main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /wd4505 /wd4201>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -pedantic>
)

target_include_directories(${PROJECT_NAME} PUBLIC
  third/
)

target_link_libraries(${PROJECT_NAME}
  enet
  EnTT::EnTT
  glfw
  imgui
  OpenGL::GL
)

if(WIN32 AND MSVC_IDE)
  target_link_libraries(${PROJECT_NAME} winmm.lib ws2_32.lib)
endif()
