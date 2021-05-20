cmake_minimum_required(VERSION 3.10)
project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third/enet)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third/glfw)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

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
  ${PROJECT_SOURCE_DIR}/third/
  ${PROJECT_SOURCE_DIR}/third/imgui
)

target_link_libraries(imgui PUBLIC glfw)

add_executable(${PROJECT_NAME}
  src/renderer.cpp
  src/shader.cpp
  src/texture.cpp
  src/main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /wd4505 /wd4201>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -pedantic>
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/third/
  ${PROJECT_SOURCE_DIR}/third/glm
  ${PROJECT_SOURCE_DIR}/third/GSL/include
  ${PROJECT_SOURCE_DIR}/third/enet/include
)

target_link_libraries(${PROJECT_NAME}
  enet
  glfw
  imgui
  OpenGL::GL
)

if(WIN32 AND MSVC_IDE)
  target_link_libraries(${PROJECT_NAME} winmm.lib ws2_32.lib)
endif()
