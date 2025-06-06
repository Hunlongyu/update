#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
//#include <plog/Logger.h>

#include <argparse/argparse.hpp>
#include <semver/semver.hpp>

#include <nlohmann/json.hpp>

#include "views/home.h"
#include "utils/utils.h"

#ifdef _DEBUG
int main(int argc, char *argv[])
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
#endif
{
    create_dir_if_not_exists(L"update");
    plog::init(plog::debug, "update/update.log");
    PLOGI << "start program.";

    Home home;
    home.Show();

    auto ver = semver::version(0, 0, 1);
    PLOGI << "version: " << ver.str();

    //argparse::ArgumentParser program("update");
    //program.add_argument("-v", "--version").help("print version").default_value(false).implicit_value(true);

    //program.parse_args(argc, argv);

    /*if (program["-v"] == true)
    {
        std::cout << ver.str() << std::endl;
    }*/

    using json = nlohmann::json;
    json j;
    j["version"] = ver.str();
    j["author"] = "hly";
    auto js = j.dump(4);
    std::cout << js << std::endl;

    sw::App::MsgLoop();
    PLOGI << "program exit.";
    return 0;
}