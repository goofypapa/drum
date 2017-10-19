cmake_minimum_required(VERSION 3.0.0)
project(gain-test VERSION 0.0.0)

option(X86_64 "编译x86_64架构程序" OFF)


set(CMAKE_C_COMPILER arm-linux-gcc)
set(CMAKE_CXX_COMPILER arm-linux-g++)

if(X86_64)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

add_definitions(-DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX)

include(CTest)
enable_testing()

link_libraries(pthread asound)

add_executable(gain-test gain-test.cpp play_wav.cpp)

if(NOT X86_64)
    file( COPY ./release.sh DESTINATION ./)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
