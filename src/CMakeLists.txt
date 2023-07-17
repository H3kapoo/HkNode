cmake_minimum_required(VERSION 3.22)
project(hkNode)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug) 

add_executable(hkNode
    main.cpp
)

add_subdirectory(base)
add_subdirectory(elements)
add_subdirectory(management)
add_subdirectory(renderer)
target_link_libraries(${PROJECT_NAME} glfw GLEW GL X11 pthread Xrandr dl)
