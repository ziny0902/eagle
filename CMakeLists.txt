cmake_minimum_required(VERSION 3.13)
project(prj)

add_subdirectory(libsrc/GLPP)
add_subdirectory(libsrc/geo)
add_subdirectory(libsrc/graphf)
add_subdirectory(libsrc/math)
add_subdirectory(libsrc/util)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)

