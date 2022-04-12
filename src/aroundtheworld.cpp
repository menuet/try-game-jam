
#include "configuration.hpp"
#include "earth.hpp"
#include "refresher.hpp"
#include "satellite.hpp"
#include "shield.hpp"
#include "universe.hpp"
#include "utilities.hpp"

namespace atw {

static Event translateEvent(ftxui::Event e)
{
  if (e.is_mouse()) {
    return {
      EventType::Mouse,
      Point{ static_cast<double>(e.mouse().x * MouseRatioX), static_cast<double>(e.mouse().y * MouseRatioY) },
    };
  }
  if (e == ftxui::Event::ArrowLeft) { return { EventType::Left }; }
  if (e == ftxui::Event::ArrowRight) { return { EventType::Right }; }
  if (e == ftxui::Event::Return) { return { EventType::Start }; }
  if (e == ftxui::Event::Custom) { return { EventType::Frame }; }
  return { EventType::Unknown };
}

void play()
{
  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  Refresher refresher{ screen, FrameInterval };

  Universe universe{ std::chrono::steady_clock::now(), randomSatellite };

  auto renderer = ftxui::Renderer([&]() { return universe.draw(); });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    universe.update(std::chrono::steady_clock::now(), translateEvent(std::move(e)));
    return false;
  });

  screen.Loop(events_catcher);
}

}// namespace atw
