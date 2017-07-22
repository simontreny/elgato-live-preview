cmake_minimum_required(VERSION 3.0)
project(elgato-x264-hook)

set(CMAKE_C_FLAGS "-Wall -g -O0")
set(GAME_CAPTURE_HD_PATH "/Applications/Game Capture HD.app")
set(X264_LIBRARY_DIR "${GAME_CAPTURE_HD_PATH}/Contents/Frameworks/ESPlayerFramework.framework/PlugIns/Elgato MPEG Support.component/Contents/Frameworks")
set(X264_LIBRARY "${X264_LIBRARY_DIR}/libx264.dylib")

set(HEADER_FILES
    frame_server.h
    x264.h
    x264_config.h)

set(SRC_FILES
    frame_server.c
    x264_hook.c)

add_library(elgato_x264_hook SHARED ${HEADER_FILES} ${SRC_FILES})
target_link_libraries(elgato_x264_hook "${X264_LIBRARY}")

configure_file(startGameCaptureHD.sh.in startGameCaptureHD.sh)
