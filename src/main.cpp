
#include "loop_based.hpp"
#include "turn_based.hpp"
#include "snake.hpp"
#include "brick.hpp"
#include <docopt/docopt.h>
#include <fmt/format.h>
#include <internal_use_only/config.hpp>
#include <map>
#include <string>

int main(int argc, const char **argv)
{
  try {
    static constexpr auto USAGE =
      R"(intro

    Usage:
          intro snake
          intro brick
          intro turn_based
          intro loop_based
          intro (-h | --help)
          intro --version
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

    if (args["turn_based"].asBool()) {
      consequence_game();
    } else if (args["loop_based"].asBool()) {
      game_iteration_canvas();
    } else if (args["snake"].asBool()) {
      snake();
    } else if (args["brick"].asBool()) {
      brick();
    }

    //    consequence_game();
  } catch (const std::exception &e) {
    fmt::print("Unhandled exception in main: {}", e.what());
  }
}
