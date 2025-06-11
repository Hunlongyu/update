#pragma once
#include <string>

struct Args
{
    std::string url;     // Github 仓库地址
    std::string version; // 当前程序的版本号
    std::string name;    // 当前程序名称
    bool setup = true;   // true: 安装包, false: 单文件
};

std::string WStringToUTF8(std::wstring_view wstr);