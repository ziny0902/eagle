#include <limits.h>
#include <unistd.h>
#include <boost/filesystem/path.hpp>
#include <util/file.h>

std::string get_fullpath(const std::string& f)
{

  std::string full_path;
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  boost::filesystem::path p(std::string(result, (count > 0) ? count : 0));
  full_path = p.remove_filename().string() + "/" + f;

  return full_path;
}
