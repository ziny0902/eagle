cmake_minimum_required(VERSION 3.13)
project(prj)

# for clang
#set(CMAKE_CXX_COMPILER_LAUNCHER "$ENV{HOME}/.local/bin/cc_args.py")
#set(CMAKE_C_COMPILER_LAUNCHER "$ENV{HOME}/.local/bin/cc_args.py")

# for rtags
set(CMAKE_CXX_COMPILER_LAUNCHER "$ENV{HOME}/.local/bin/rc_launcher.sh")
set(CMAKE_C_COMPILER_LAUNCHER "$ENV{HOME}/.local/bin/rc_launcher.sh")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project
add_subdirectory(libsrc/GLPP)
add_subdirectory(libsrc/geo)
add_subdirectory(libsrc/graphf)
add_subdirectory(libsrc/math)
add_subdirectory(libsrc/util)
add_subdirectory(src)

# for rtags
add_custom_command(
OUTPUT ${CMAKE_BINARY_DIR}/.rdm_out
COMMAND rc -J ${CMAKE_BINARY_DIR}/compile_commands.json && /usr/bin/touch ${CMAKE_BINARY_DIR}/.rdm_out
DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
  )
add_custom_target(rdm ALL DEPENDS ${CMAKE_BINARY_DIR}/.rdm_out)

# test
enable_testing()
add_subdirectory(test)

