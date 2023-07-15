cmake_minimum_required(VERSION 3.22)
project(hkNode)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../debug) 

add_executable(hkNode
    main.cpp
    base/HkNode.cpp
    elements/HkWindowFrame.cpp
    elements/HkButton.cpp
    elements/HkContainer.cpp
    management/HkSceneManagement.cpp
    renderer/HkShader.cpp
    )
target_link_libraries(hkNode glfw GLEW GL X11 pthread Xrandr dl)