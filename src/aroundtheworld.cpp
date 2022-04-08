
#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "shield.hpp"
#include "universe.hpp"
#include "utilities.hpp"

namespace atw {

void play()
{
  std::atomic<bool> refresh_ui_continue = true;

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      std::this_thread::sleep_for(FrameInterval);
      screen.PostEvent(ftxui::Event::Custom);
    }
  });

  Universe universe{};

  auto renderer = ftxui::Renderer([&]() {
    universe.update();
    return universe.draw();
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    universe.onEvent(e);
    return false;
  });

  screen.Loop(events_catcher);

  refresh_ui_continue = false;
  refresh_ui.join();
}

}// namespace atw
