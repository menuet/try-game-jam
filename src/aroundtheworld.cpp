
#include "utilities.hpp"
#include "configuration.hpp"
#include "shield.hpp"
#include "asteroid.hpp"
#include "earth.hpp"
#include "universe.hpp"

namespace atw {

void play()
{
  Universe universe{};

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  auto renderer = ftxui::Renderer([&]() {
    universe.update();
    return universe.draw();
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    universe.onEvent(e);
    return false;
  });

  std::atomic<bool> refresh_ui_continue = true;

  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      std::this_thread::sleep_for(FrameInterval);
      screen.PostEvent(ftxui::Event::Custom);
    }
  });

  screen.Loop(events_catcher);

  refresh_ui_continue = false;
  refresh_ui.join();
}

}
