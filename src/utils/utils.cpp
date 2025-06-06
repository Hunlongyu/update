#include "utils.h"
#include <filesystem>

void create_dir_if_not_exists(const std::wstring &path)
{
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}