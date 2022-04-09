
#pragma once

#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "shield.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <functional>

namespace atw {

class Universe
{
  double fps{};
  int counter{};
  std::size_t points{};
  std::chrono::steady_clock::time_point last_update_time{};
  std::chrono::steady_clock::time_point last_asteroids_update_time{};
  std::chrono::steady_clock::time_point last_asteroids_creation_time{};
  Earth earth{};
  Shield shield{};
  std::vector<Asteroid> asteroids{};
  std::function<Asteroid()> createAsteroid{};

public:
  explicit Universe(std::chrono::steady_clock::time_point now, std::function<Asteroid()> asteroidCreator)
    : last_update_time{ now }, last_asteroids_update_time{ now }, last_asteroids_creation_time{ now }, createAsteroid{
        std::move(asteroidCreator)
      }
  {
    asteroids.resize(InitialAsteroidsCount);
    std::generate(begin(asteroids), end(asteroids), createAsteroid);
  }

  void update(std::chrono::steady_clock::time_point now, const Point &mouse)
  {
    if (!earth.update(asteroids)) return;

    ++counter;
    const auto new_time = now;
    const auto elapsed_time = new_time - last_update_time;
    last_update_time = new_time;
    fps = 1.0
          / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
             / MicrosSecondsPerSecond);

    shield.update(mouse);

    if (new_time - last_asteroids_update_time < FrameInterval) return;
    last_asteroids_update_time = new_time;

    for (auto &asteroid : asteroids)
      if (asteroid.update(shield)) points += asteroids.size();

    if (new_time - last_asteroids_creation_time < AsteroidCreationInterval) return;
    last_asteroids_creation_time = new_time;

    asteroids.push_back(createAsteroid());
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
        ftxui::text("FPS: " + std::to_string(fps)),
        ftxui::text("Asteroids: " + std::to_string(asteroids.size())),
        ftxui::text("Points: " + std::to_string(points)) }) });
  }

  // For unit tests only
  std::size_t getPoints() const { return points; }
  const Shield &getShield() const { return shield; }
  const std::vector<Asteroid> getAsteroids() const { return asteroids; }
};

}// namespace atw
