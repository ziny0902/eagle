#include <iostream>
#include <util/font_util.h>
#include <fontconfig/fontconfig.h>

int main()
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft)){
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return -1;
  }
  const char *font = "D2Cording";
  FT_Face face = load_face_from_fontname(ft, font);
  FT_Done_FreeType(ft);

  if(face == NULL) return -1;
  std::cout << "font " << font << " loading success!"
  <<std::endl;
  return  0;
}
