
#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "refresher.hpp"
#include "shield.hpp"
#include "universe.hpp"
#include "utilities.hpp"
#include <chrono>

namespace atw {

void play()
{
  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  Refresher refresher{ screen, FrameInterval };

  Universe universe{ std::chrono::steady_clock::now(), randomAsteroid };
  Point mouse{};

  auto renderer = ftxui::Renderer([&]() {
    universe.update(std::chrono::steady_clock::now(), mouse);
    return universe.draw();
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    if (e.is_mouse()) {
      mouse.x = e.mouse().x * MouseRatioX;
      mouse.y = e.mouse().y * MouseRatioY;
    }
    return false;
  });

  screen.Loop(events_catcher);
}

}// namespace atw
