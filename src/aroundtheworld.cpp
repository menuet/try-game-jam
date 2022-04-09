
#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "refresher.hpp"
#include "shield.hpp"
#include "universe.hpp"
#include "utilities.hpp"

namespace atw {

void play()
{
  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  Refresher refresher{ screen, FrameInterval };

  Universe universe{ std::chrono::steady_clock::now(), randomAsteroid };

  auto renderer = ftxui::Renderer([&]() { return universe.draw(); });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    std::optional<Point> mouse{};
    if (e.is_mouse())
      mouse.emplace(static_cast<double>(e.mouse().x * MouseRatioX), static_cast<double>(e.mouse().y * MouseRatioY));
    universe.update(std::chrono::steady_clock::now(), mouse);
    return false;
  });

  screen.Loop(events_catcher);
}

}// namespace atw
