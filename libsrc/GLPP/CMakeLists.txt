# CMake instructions to make the static lib
project(prj)

cmake_minimum_required(VERSION 3.13)


ADD_LIBRARY( glpp STATIC
            shader_util.cpp
            glBuffer.cpp
            vertex_arrays.cpp
            shader.cpp
            texture.cpp
            # window.cpp )
						)

target_include_directories(glpp PUBLIC ../../include) 

install( TARGETS glpp
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)

