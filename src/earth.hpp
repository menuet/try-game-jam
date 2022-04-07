
#pragma once

#include "utilities.hpp"
#include "configuration.hpp"
#include "asteroid.hpp"
#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <vector>
#include <algorithm>

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
    canvas.DrawBlockCircleFilled((int)EarthCenter.x, (int)EarthCenter.y, EarthRadius);
    canvas.DrawPointCircle((int)EarthCenter.x, (int)EarthCenter.y, ShieldRadius);
    if (is_destroyed)
      canvas.DrawText((int)EarthCenter.x - 20, (int)EarthCenter.y, "BOOOOOOOOOOOOOOOOOOOOOOM", ftxui::Color::Red);
  }
};

}
