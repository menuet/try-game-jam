
#pragma once

#include "asteroid.hpp"
#include "configuration.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <vector>

namespace atw {

class Earth
{
  bool is_destroyed{};

public:
  bool update(const std::vector<Asteroid> &asteroids)
  {
    is_destroyed = std::any_of(std::begin(asteroids), std::end(asteroids), [](const auto &a) {
      return distance(a.position, EarthCenter) <= EarthRadius + AsteroidRadius;
    });
    return !is_destroyed;
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockCircleFilled(static_cast<int>(EarthCenter.x),
      static_cast<int>(EarthCenter.y),
      EarthRadius,
      is_destroyed ? ftxui::Color::Red : ftxui::Color::Blue);
    canvas.DrawPointCircle(static_cast<int>(EarthCenter.x), static_cast<int>(EarthCenter.y), ShieldRadius);
    if (is_destroyed)
      canvas.DrawText(static_cast<int>(EarthCenter.x) - 20,
        static_cast<int>(EarthCenter.y),
        "BOOOOOOOOOOOOOOOOOOOOOOM",
        ftxui::Color::Red);
  }
};

}// namespace atw
