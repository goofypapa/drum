cmake_minimum_required(VERSION 3.0.0)
project(rhythm-test VERSION 0.0.0)

option(X86_64 "编译x86_64架构程序" OFF)

set(CMAKE_C_COMPILER arm-linux-gcc)
set(CMAKE_CXX_COMPILER arm-linux-g++)

if(X86_64)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

include(CTest)
enable_testing()

include_directories(../../include)
include_directories(../../include/core)
include_directories(../../deps)

add_executable(rhythm-test rhythm-test.cpp 
                ../../src/core/ws_log.cpp
                ../../src/core/json.cpp
                ../../src/core/play_wav.cpp
)

add_executable(rhythm_parse rhythm_parse.cpp 
                ../../src/core/ws_log.cpp
                ../../src/core/json.cpp
)

target_link_libraries(rhythm-test pthread asound)

if(NOT X86_64)
    file( COPY ./release.sh DESTINATION ./)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
