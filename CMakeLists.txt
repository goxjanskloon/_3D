cmake_minimum_required(VERSION 3.5.0)
project(_3D VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

link_directories(${PROJECT_SOURCE_DIR}/EGE)

add_executable(_3D main.cpp _3D.cpp)

target_link_libraries(_3D libgraphics64.a libgdiplus.a libuuid.a libmsimg32.a libgdi32.a libimm32.a libole32.a liboleaut32.a libwinmm.a)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
