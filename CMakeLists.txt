cmake_minimum_required(VERSION 3.21...3.31)

project(Physarum VERSION 1.0 DESCRIPTION "" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_subdirectory(src bin)

target_include_directories(Physarum PRIVATE include)

include(libs/Libraries.cmake)
build_libraries()

link_libraries(Physarum "glad;glfw;imgui")
