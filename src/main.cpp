#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Log.h>
#include <plog/Logger.h>

#include <argparse/argparse.hpp>
#include <semver/semver.hpp>

#include <nlohmann/json.hpp>

#include <SimpleWindow.h>

#ifdef _DEBUG
int main(int argc, char *argv[])
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
#endif
{
    plog::init(plog::debug, "update.log");
    PLOGI << "start the update program.";

    auto ver = semver::version(0, 0, 1);
    PLOGI << "version: " << ver.str();

    argparse::ArgumentParser program("update");
    program.add_argument("-v", "--version").help("print version").default_value(false).implicit_value(true);

    program.parse_args(argc, argv);

    if (program["-v"] == true)
    {
        std::cout << ver.str() << std::endl;
    }

    using json = nlohmann::json;
    json j;
    j["version"] = ver.str();
    j["author"] = "hly";
    auto js = j.dump(4);
    std::cout << js << std::endl;

    sw::Window mw;
    mw.Show();

    return sw::App::MsgLoop();
}