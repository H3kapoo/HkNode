cmake_minimum_required(VERSION 3.22)
# project(hkNode)
project(windowFrameWithLabel)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g") # debug mode
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2") # optimized
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pg") # profiling (gprof hkNode gmon.out | gprof2dot -w -s --color-nodes-by-selftime | dot -Gdpi=100 -Tpng -o output.png)

#valgrind --tool=massif ./hkNode 
#ms_print massif.out.454065 > masssif.txt

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug) 

include_directories ("/usr/include/freetype2") # linux specific

# add_executable(${PROJECT_NAME}
#     main.cpp
# )

add_executable(${PROJECT_NAME}
    ../examples/windowFrameWithLabel.cpp
)

add_subdirectory(base)
add_subdirectory(elements)
add_subdirectory(management)
add_subdirectory(contexts)
add_subdirectory(renderer)
add_subdirectory(utils)
target_link_libraries(${PROJECT_NAME} glfw GLEW GL X11 pthread Xrandr dl freetype)

#TODO: See how I can build multiple execs