cmake_minimum_required(VERSION 3.15)
project(CLIENT C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC -lwsock32)
