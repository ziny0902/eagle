#include <iostream>
#include <fontconfig/fontconfig.h>
#include <util/font_util.h>

FT_Face load_face_from_fontname(FT_Library ft, std::string &&f_name)
{
  FcBool ret;

  ret = FcInitReinitialize();
  if(ret == false){
    std::cout << "(" << __FILE__ << __LINE__<< ")" 
    << "FcInitReinitialize() fail\n"; 
    return NULL;
  }

  FcPattern* pat = FcNameParse((const FcChar8*)(f_name.c_str()));
  if(pat == NULL) {
    std::cout << "(" << __FILE__ << __LINE__<< ")" 
    << "FcNameParse() fail\n"; 
    // FcConfigDestroy(config);
    FcFini();
    return NULL;
  }

  ret = FcConfigSubstitute(NULL, pat, FcMatchPattern);
  FcDefaultSubstitute (pat);
  if(ret == false)
    std::cout << "FcConfigSubstitute() fails" << std::endl;

  std::string font_file;
  {
  // find the font
  FcResult result;
  FcPattern* fpat= FcFontMatch(NULL, pat, &result);
  if (fpat)
  {
     FcChar8* file = NULL;
     if (FcPatternGetString(fpat, FC_FILE, 0, &file) == FcResultMatch)
     {
        // save the file to another std::string
        font_file.append((char *)file);
#ifdef __DEBUG__
        std::cout << "(" << __FILE__ << __LINE__<< ")" 
        << font_file << std::endl; 
#endif
        
     }
  }
  FcPatternDestroy(fpat);
  }
  FcPatternDestroy(pat);
  //
  // TODO. I don't know why but in thread environment, 
  // a Call to FcFini() cause a sigabort.
  // FcFini();
  //

  FT_Face face = NULL;
  if(font_file.length() == 0) {
      std::cout << "(" << __FILE__ << __LINE__<< ")" 
      << "font don't exist: "<< f_name << std::endl; 
  }
  else {
#ifdef __DEBUG__
    std::cout << "(" << __FILE__ << __LINE__<< ")" 
    << "font name: "<< f_name << std::endl; 
#endif

    FT_Error ft_err;
    if (ft_err =FT_New_Face(ft, font_file.c_str(), 0, &face)){
      std::cout << "ERROR::FREETYPE: Failed to load font: " 
      << ft_err
      << std::endl; 
    }
  }
  return face;
}
