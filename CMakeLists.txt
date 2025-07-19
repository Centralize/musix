cmake_minimum_required(VERSION 3.10)
project(Musix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswresample)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# Add include directories
include_directories(
    ${FFMPEG_INCLUDE_DIRS}
    ${PORTAUDIO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    include
)

# Add source files
set(SOURCES
    src/main.cpp
    src/AudioEngine/AudioEngine.cpp
    src/CLI/CLI.cpp
    src/PlaylistManager/PlaylistManager.cpp
)

# Add the executable
add_executable(musix ${SOURCES})

# Link libraries
target_link_libraries(musix
    ${FFMPEG_LIBRARIES}
    ${PORTAUDIO_LIBRARIES}
)
