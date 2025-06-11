#include "utils.h"
#include <filesystem>
#include <windows.h>

void create_dir_if_not_exists(const std::wstring &path)
{
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

std::string WStringToUTF8(const std::wstring_view wstr)
{
    if (wstr.empty())
        return {};

    const int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);

    std::string utf8_str(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), utf8_str.data(), size, nullptr, nullptr);

    return utf8_str;
}