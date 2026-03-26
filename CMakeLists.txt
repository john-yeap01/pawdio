cmake_minimum_required(VERSION 3.10.0)
project(pawdio VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty/portaudio)
add_subdirectory(3rdparty/stretch)

add_executable(pawdio
    src/main.cpp
)

target_link_libraries(pawdio
    PRIVATE
        portaudio
        signalsmith-stretch
)