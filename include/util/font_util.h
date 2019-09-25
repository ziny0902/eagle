#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

extern "C" FT_Face load_face_from_fontname(FT_Library ft, std::string &&f_name);


