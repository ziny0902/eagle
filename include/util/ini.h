#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glib.h>

class IniManager {
public:
  IniManager(std::string &&f);
  ~IniManager();
  const std::shared_ptr<std::vector<double>> get_double_list
    (const std::string& group, const std::string& key);
  const std::shared_ptr<std::string> get_string
    (const std::string& group, const std::string& key);
  const double get_double
    (const std::string& group, const std::string& key);
  const bool get_boolean
    (const std::string& group, const std::string& key);
  const int get_integer
    (const std::string& group, const std::string& key);

private:
  GKeyFile* m_gkf; /* Notice we declared a pointer */
};
