cmake_minimum_required(VERSION 3.10)
project(VENG C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_library(VENG STATIC
    src/VENG.c
    src/VENG_listeners.c
)

target_include_directories(VENG PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(VENG
    SDL2
    SDL2_image
    m
)
