#include <glib/gprintf.h>
#include <signal.h>
#include "ini.h"

IniManager::IniManager(std::string &&f)
{
  g_autoptr(GError) error = NULL;
  m_gkf = g_key_file_new();

  if (!g_key_file_load_from_file(m_gkf, f.c_str(), G_KEY_FILE_NONE, NULL)){
    g_warning ("Error loading key file: %s", error->message);
    raise(SIGTRAP);
  }
}

IniManager::~IniManager()
{
  g_key_file_free (m_gkf);
}

const std::shared_ptr<std::string> IniManager::get_string
  (const std::string& group, const std::string& key)
{
  g_autoptr(GError) error = NULL;
  g_autofree gchar *val = g_key_file_get_string (m_gkf, group.c_str(), key.c_str(), &error);
  if (val == NULL &&
    !g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
  {
    g_warning ("Error finding key in key file: %s", error->message);
    return nullptr;
  }
  if(val == NULL) return nullptr;
  return std::make_shared<std::string>(val);
}

const std::shared_ptr<std::vector<double>> IniManager::get_double_list
  (const std::string& group, const std::string& key)
{
  g_autoptr(GError) error = NULL;
  gsize length;

  g_autofree gdouble *val = g_key_file_get_double_list(m_gkf, group.c_str(), key.c_str(), &length, &error);
  if (val == NULL &&
    !g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
  {
    g_warning ("Error finding key in key file: %s", error->message);
    return nullptr;
  }
  if(val == NULL) return nullptr;

  std::shared_ptr<std::vector<double>> v = std::make_shared<std::vector<double>>();
  for(int i = 0; i < length; i++) {
    v->push_back(val[i]);
  }

  return v;
}

const double IniManager::get_double
  (const std::string& group, const std::string& key)
{
  g_autoptr(GError) error = NULL;

  gdouble val = g_key_file_get_double(m_gkf, group.c_str(), key.c_str(), &error);
  if (g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
  {
    g_warning ("Error finding key in key file: %s", error->message);
    return 0;
  }

  return val;
}

const bool IniManager::get_boolean
    (const std::string& group, const std::string& key)
{
  g_autoptr(GError) error = NULL;

  gboolean val = g_key_file_get_boolean(m_gkf, group.c_str(), key.c_str(), &error);
  if (g_error_matches (error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
  {
    g_warning ("Error finding key in key file: %s", error->message);
    return false;
  }

  return val;
}
