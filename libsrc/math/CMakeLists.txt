# CMake instructions to make the static lib

cmake_minimum_required(VERSION 3.13)


ADD_LIBRARY( math_helper SHARED
multivCalculus.cpp
)

# add_executable( math_test_bin main.cpp)
# target_link_libraries( math_test_bin math_helper gsl)

target_include_directories(math_helper PUBLIC ../../include) 

install( TARGETS math_helper 
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../lib
)


