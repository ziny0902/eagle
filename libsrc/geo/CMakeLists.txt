# CMake instructions to make the static lib

cmake_minimum_required(VERSION 3.13)


ADD_LIBRARY( geowrap STATIC
geometry_object_3d.cpp
coordinate_convert.cpp
)

target_include_directories(geowrap PUBLIC ../../include) 

install( TARGETS geowrap
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)

