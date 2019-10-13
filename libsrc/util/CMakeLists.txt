# CMake instructions to make the static lib

cmake_minimum_required(VERSION 3.13)

find_package(PkgConfig)
pkg_check_modules(FREETYPE freetype2)
pkg_check_modules(GLIB glib-2.0)

ADD_LIBRARY( user_util STATIC
font_util.cpp
ini.cpp
file.cpp
)

target_include_directories(user_util PUBLIC 
                        ${FREETYPE_INCLUDE_DIRS}
                        ${GLIB_INCLUDE_DIRS}
                        ../../include
                        )

install( TARGETS user_util 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)

