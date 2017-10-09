cmake_minimum_required(VERSION 3.0.0)
project(wifi-test VERSION 0.0.0)

set(CMAKE_C_COMPILER arm-linux-gcc)
set(CMAKE_CXX_COMPILER arm-linux-g++)

add_definitions(-DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX)

include(CTest)
enable_testing()

set(WpaDir "../../deps/wpa_supplicant/src")

include_directories(${WpaDir}/common)
include_directories(${WpaDir}/utils)

link_libraries(pthread)

add_executable(wifi-test wifi-test.cpp ${WpaDir}/common/wpa_ctrl.c ${WpaDir}/utils/os_unix.c)

file( COPY ./release.sh DESTINATION ./)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
