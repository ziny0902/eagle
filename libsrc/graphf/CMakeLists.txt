# CMake instructions to make the static lib

cmake_minimum_required(VERSION 3.13)


ADD_LIBRARY( graphf SHARED
func_collection.cpp
)

install( TARGETS graphf 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)

