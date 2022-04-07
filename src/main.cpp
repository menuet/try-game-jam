
#include "aroundtheworld.hpp"
#include <docopt/docopt.h>
#include <fmt/format.h>
#include <internal_use_only/config.hpp>
#include <map>
#include <string>

int main(int argc, const char **argv)
{
  try {
    static constexpr auto USAGE =
      R"(aroundtheworld

    Usage:
          aroundtheworld
          aroundtheworld (-h | --help)
          aroundtheworld --version
 Options:
          -h --help     Show this screen.
          --version     Show version.
)";

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
      { std::next(argv), std::next(argv, argc) },
      true,// show help if requested
      fmt::format("{} {}",
        try_game_jam::cmake::project_name,
        try_game_jam::cmake::project_version));// version string, acquired
                                               // from config.hpp via CMake

    atw::play();

    //    consequence_game();
  } catch (const std::exception &e) {
    fmt::print("Unhandled exception in main: {}", e.what());
  }
}
