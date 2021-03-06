cmake_minimum_required(VERSION 3.0)
project(elgato-live-preview)

set(CMAKE_C_FLAGS "-Wall -g -O3")
set(CMAKE_CXX_FLAGS "-Wall -g -O3 -std=c++11")

find_package(GLEW REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(HEADER_FILES
    frame.h
    frame_client.h
    frame_queue.h
    frame_renderer.h
    gl_includes.h
    non_copyable.h
    post_process.h
    render_to_texture.h
    screen_aligned_quad.h
    shader.h
    utils.h)

set(SRC_FILES
    frame.cpp
    frame_client.cpp
    frame_queue.cpp
    frame_renderer.cpp
    main.cpp
    post_process.cpp
    render_to_texture.cpp
    screen_aligned_quad.cpp
    shader.cpp
    utils.cpp)

include_directories(${OPENGL_INCLUDE_DIR})
add_executable(elgato_live_preview ${HEADER_FILES} ${SRC_FILES})
target_link_libraries(elgato_live_preview ${OPENGL_gl_LIBRARY} GLEW::GLEW glfw)

configure_file(shaders/screenAlignedQuad.glsl shaders/screenAlignedQuad.glsl COPYONLY)
configure_file(shaders/sharpen.glsl shaders/sharpen.glsl COPYONLY)
configure_file(shaders/uyvyToRgb.glsl shaders/uyvyToRgb.glsl COPYONLY)
configure_file(shaders/yuvPlanarToRgb.glsl shaders/yuvPlanarToRgb.glsl COPYONLY)
