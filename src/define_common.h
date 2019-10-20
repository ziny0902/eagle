#ifndef __DEFINE__COMMON_H__
#define __DEFINE__COMMON_H__

namespace app_common{

constexpr unsigned short PLOT3D_ENABLED  = 0x01;
constexpr unsigned short MESH_ENABLED  = 0x02;
constexpr unsigned short FIGURE_ENABLED  = 0x04;
constexpr unsigned short VECTOR_ENABLED  = 0x08;

enum app_gl_object {
  none
  , mesh
  , plot3d
  , vector3d
  , plane3d
};

}

#endif
