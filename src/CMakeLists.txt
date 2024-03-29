cmake_minimum_required(VERSION 3.13)
project(prj)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(FREETYPE freetype2)
pkg_check_modules(FONTCONFIG fontconfig)

add_compile_definitions(
)
link_directories( ${GTKMM_LIBRARY_DIRS} )

add_executable(graph3d
                    GlApp.v0.1.cpp
                    main.cpp
                    axis.3D.cpp 
                    GtkWindow.cpp
                    mesh.cpp
                    plot3d.cpp
                    vector3d.cpp
                    object3d.cpp
                    glWidgetWindow.cpp
                    VectorAddDialog.cpp
                    Plane3dAddDig.cpp
                    plane3d.cpp
                    Highlight.cpp
                    Plot3dDig.cpp
                    )

target_include_directories(graph3d PUBLIC 
									${CMAKE_CURRENT_SOURCE_DIR}/include
                  ${FREETYPE_INCLUDE_DIRS}
                  ${GTKMM_INCLUDE_DIRS})


target_link_libraries(graph3d 
  epoxy 
  GLU 
  glpp 
  geowrap 
  user_util
  math_helper
  boost_iostreams 
  boost_serialization 
	boost_system
	boost_filesystem
  gsl
  dl 
  m 
  ${FREETYPE_LIBRARIES} 
  ${FONTCONFIG_LIBRARIES}
  ${GTKMM_LIBRARIES}
)

add_custom_command(TARGET graph3d 
POST_BUILD
COMMAND $ENV{HOME}/.local/bin/gen_clang_complete.sh ../../.clang_complete
)


