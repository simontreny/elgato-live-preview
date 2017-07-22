cmake_minimum_required(VERSION 3.0)
project(elgato-live-preview)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_C_FLAGS "-Wall -g -O3")
set(CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11")

set(HEADER_FILES
    frame.h
    frame_client.h
    frame_queue.h
    frame_renderer.h
    linmath.h)

set(SRC_FILES
    frame.cpp
    frame_client.cpp
    frame_queue.cpp
    frame_renderer.cpp
    main.cpp)

include_directories(${OPENGL_INCLUDE_DIR})
add_executable(elgato_live_preview ${HEADER_FILES} ${SRC_FILES})
target_link_libraries(elgato_live_preview ${OPENGL_gl_LIBRARY} glfw)
