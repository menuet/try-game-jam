
#pragma once

#include "asteroid.hpp"
#include "configuration.hpp"
#include "earth.hpp"
#include "shield.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <functional>
#include <optional>

namespace atw {

enum class State {
  Intro = 1,
  Play = 2,
  End = 3,
};

class Universe
{
  std::size_t points{};
  std::chrono::steady_clock::time_point last_asteroid_creation_time{};
  Earth earth{};
  Shield shield{};
  std::vector<Asteroid> asteroids{};
  std::function<Asteroid()> createAsteroid{};
  State state{ State::Intro };

public:
  explicit Universe(std::chrono::steady_clock::time_point now, std::function<Asteroid()> asteroidCreator)
    : last_asteroid_creation_time{ now }, createAsteroid{ std::move(asteroidCreator) }
  {
    asteroids.resize(InitialAsteroidsCount);
    std::generate(begin(asteroids), end(asteroids), createAsteroid);
  }

  void update(std::chrono::steady_clock::time_point now, const std::optional<Point> &mouse)
  {
    if (state == State::End) return;

    if (mouse) {
      shield.update(*mouse);
      return;
    }

    for (auto &asteroid : asteroids)
      if (asteroid.update(shield)) points += asteroids.size();

    const auto isEnd = !earth.update(asteroids);
    if (isEnd) {
      state = State::End;
      return;
    }

    if (now - last_asteroid_creation_time >= AsteroidCreationInterval) {
      last_asteroid_creation_time = now;
      asteroids.push_back(createAsteroid());
    }
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
      ftxui::vbox({ ftxui::text("Asteroids: " + std::to_string(asteroids.size())),
        ftxui::text("Points: " + std::to_string(points)) }) });
  }

  // For unit tests only
  std::size_t getPoints() const { return points; }
  const Shield &getShield() const { return shield; }
  const std::vector<Asteroid> getAsteroids() const { return asteroids; }
};

}// namespace atw
