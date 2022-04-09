
#pragma once

#include "satellite.hpp"
#include "configuration.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <vector>

namespace atw {

class Earth
{
  bool is_destroyed{};

public:
  bool update(const std::vector<Satellite> &satellites)
  {
    if (is_destroyed) return false;
    is_destroyed =
      std::any_of(std::begin(satellites), std::end(satellites), [](const auto &a) { return a.isNearEarth(); });
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
