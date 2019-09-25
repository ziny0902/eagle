
cmake_minimum_required(VERSION 3.13)

add_executable( math_test_bin main.cpp)
target_link_libraries( math_test_bin math_helper gsl)

target_include_directories(math_helper PUBLIC ../include) 

add_test(NAME math_test COMMAND math_test_bin WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test) 


#font config test.
find_package(PkgConfig)
pkg_check_modules(FREETYPE freetype2)
pkg_check_modules(FONTCONFIG fontconfig)

add_executable(fontconfig_test  fontconfig_test.cpp)

target_include_directories(fontconfig_test PUBLIC ${FREETYPE_INCLUDE_DIRS} ../include) 

target_link_libraries(
  fontconfig_test 
  user_util
  ${FREETYPE_LIBRARIES} 
  ${FONTCONFIG_LIBRARIES}
)
