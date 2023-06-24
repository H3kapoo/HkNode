cmake_minimum_required(VERSION 3.22)
project(test_hkNode)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../artifacts) 

# Build Src along with Test
add_subdirectory(/home/hekapoo/hkNode/src /home/hekapoo/hkNode/debug)

add_executable(test_hkNode
    hkTestFw/HkTestBase.cpp
    test.cpp
    tests/base/HkNode.test.cpp
    ../src/base/HkNode.cpp

    )
# target_link_libraries(hkNode glfw GLEW GL X11 pthread Xrandr dl)