
#pragma once

#include "utilities.hpp"
#include "configuration.hpp"
#include "asteroid.hpp"
#include "shield.hpp"
#include "earth.hpp"

namespace atw {

class Universe
{
  double fps = 0;
  int counter = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_asteroid_time = last_time;
  Point mouse{};
  Earth earth{};
  Shield shield{};
  std::vector<Asteroid> asteroids{
    { { .x = EarthCenter.x / 2, .y = 0. }, { .dx = 1.0, .dy = 1.0 } },
    { { .x = EarthCenter.x * 3 / 2, .y = UniverseHeight }, { .dx = 1.0, .dy = -1.0 } },
  };

public:
  void onEvent(ftxui::Event &e)
  {
    if (e.is_mouse()) {
      mouse.x = e.mouse().x * MouseRatioX;
      mouse.y = e.mouse().y * MouseRatioY;
    }
  }

  void update()
  {
    if (!earth.update(asteroids)) return;

    ++counter;
    const auto new_time = std::chrono::steady_clock::now();
    const auto elapsed_time = new_time - last_time;
    last_time = new_time;
    fps = 1.0
          / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
             / MicrosSecondsPerSecond);

    shield.update(mouse);

    if (new_time - last_asteroid_time < FrameInterval) return;
    last_asteroid_time = new_time;

    for (auto &asteroid : asteroids) asteroid.update(shield);
  }

  auto draw() const
  {
    auto universeComponent = ftxui::Renderer([&] {
      auto canvas = ftxui::Canvas(UniverseWidth, UniverseHeight);
      earth.draw(canvas);
      shield.draw(canvas);
      for (const auto &asteroid : asteroids) asteroid.draw(canvas);
      return ftxui::canvas(std::move(canvas));
    });

    return ftxui::hbox({ universeComponent->Render() | ftxui::borderDouble,
      ftxui::separator(),
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)),
        ftxui::text("FPS: " + std::to_string(fps)) }) });
  }
};

}
