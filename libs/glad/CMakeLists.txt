cmake_minimum_required(VERSION 3.28)
project(glad C)

set(CMAKE_C_STANDARD 99)

set(GLAD_SOURCES
        src/glad.c
)

add_library(${PROJECT_NAME} STATIC ${GLAD_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)
