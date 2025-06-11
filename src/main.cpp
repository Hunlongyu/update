#include "utils/utils.h"
#include "views/home.h"
#include <argparse/argparse.hpp>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>

#ifdef _DEBUG
int main(int argc, char *argv[])
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
#endif
{
    setlocale(LC_ALL, "chs");
    SetConsoleOutputCP(CP_UTF8);
    _mkdir("updater");
    plog::init(plog::debug, "updater/updater.log");
    PLOGI << "start program.";

    Args m_args;
#ifdef _DEBUG
    m_args.version = "0.0.1";
    m_args.name = "Textify";
    m_args.url = "https://github.com/Hunlongyu/Textify";
    m_args.setup = true;
#endif

#ifndef _DEBUG
    try
    {
        argparse::ArgumentParser program("updater");
        program.add_argument("-v", "--version").required().help("current program version.");
        program.add_argument("-u", "--url").required().help("github repo url.");
        program.add_argument("-n", "--name").required().help("current program name.");
        program.add_argument("-s", "--setup").default_value(true).implicit_value(true).nargs(0);
        int argc;
        LPWSTR *argv_w = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (argv_w == nullptr)
        {
            return 1;
        }
        std::vector<std::string> argv_strings;
        std::vector<const char *> argv_char;
        argv_strings.reserve(argc);
        argv_char.reserve(argc);
        for (int i = 0; i < argc; i++)
        {
            argv_strings.emplace_back(sw::Utils::ToMultiByteStr(argv_w[i]));
            argv_char.push_back(argv_strings.back().c_str());
        }
        program.parse_args(static_cast<int>(argv_char.size()), argv_char.data());
        LocalFree(argv_w);
        m_args.url = program.get<std::string>("--url");
        m_args.version = program.get<std::string>("--version");
        m_args.name = program.get<std::string>("--name");
        m_args.setup = program.get<bool>("--setup");
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        PLOGE << err.what();
        return 1;
    }
#endif

    Home home(m_args);
    home.Show();

    sw::App::MsgLoop();
    PLOGI << "program exit.";
    return 0;
}