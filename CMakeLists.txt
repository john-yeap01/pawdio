cmake_minimum_required(VERSION 3.10.0)
project(pawdio VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty/portaudio)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RUBBERBAND REQUIRED rubberband)

add_executable(pawdio
    src/main.cpp
)

target_include_directories(pawdio PRIVATE
    ${RUBBERBAND_INCLUDE_DIRS}
)

target_link_directories(pawdio PRIVATE
    ${RUBBERBAND_LIBRARY_DIRS}
)

target_link_libraries(pawdio PRIVATE
    portaudio
    ${RUBBERBAND_LIBRARIES}
)