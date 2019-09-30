# CMake instructions to make the static lib
project(prj)

cmake_minimum_required(VERSION 3.13)

find_package(PkgConfig)
pkg_check_modules(FREETYPE freetype2)
pkg_check_modules(GLIB glib-2.0)


ADD_LIBRARY( glpp STATIC
            shader_util.cpp
            glBuffer.cpp 
            vertex_arrays.cpp 
            shader.cpp 
            texture.cpp 
            glWidget.cpp
            glWidgetManager.cpp
            gl_text.cpp
            gl_resource_manager.cpp
            )

target_include_directories(glpp PUBLIC 
                          ${GLIB_INCLUDE_DIRS}
                          ${FREETYPE_INCLUDE_DIRS}
                          ../../include
                          ) 

install( TARGETS glpp
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)

