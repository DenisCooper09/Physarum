cmake_minimum_required(VERSION 3.21...3.31)

project(imgui VERSION 1.91.9)

set(SOURCES
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_impl_glfw.cpp
        src/imgui_impl_opengl3.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp
)

add_library(imgui STATIC ${SOURCES})

target_include_directories(imgui PUBLIC include)

include(../Libraries.cmake)
link_libraries(imgui "glad;glfw")
