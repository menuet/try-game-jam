
#pragma once

#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "shield.hpp"
#include "utilities.hpp"

namespace atw {

class Universe
{
  double fps{};
  int counter{};
  const std::chrono::steady_clock::time_point initial_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_update_time = initial_time;
  std::chrono::steady_clock::time_point last_asteroids_update_time = last_update_time;
  std::chrono::steady_clock::time_point last_asteroids_creation_time = last_update_time;
  Point mouse{};
  Earth earth{};
  Shield shield{};
  std::vector<Asteroid> asteroids{ randomAsteroid() };

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
    const auto elapsed_time = new_time - last_update_time;
    last_update_time = new_time;
    fps = 1.0
          / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
             / MicrosSecondsPerSecond);

    shield.update(mouse);

    if (new_time - last_asteroids_update_time < FrameInterval) return;
    last_asteroids_update_time = new_time;

    for (auto &asteroid : asteroids) asteroid.update(shield);

    if (new_time - last_asteroids_creation_time < AsteroidCreationInterval) return;
    last_asteroids_creation_time = new_time;

    asteroids.push_back(randomAsteroid());
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
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)), ftxui::text("FPS: " + std::to_string(fps)) }) });
  }
};

}// namespace atw
