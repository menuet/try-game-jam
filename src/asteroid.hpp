
#pragma once

#include "configuration.hpp"
#include "utilities.hpp"
#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive

namespace atw {

struct Asteroid
{
  Point position{};
  Offset velocity{};
  double distanceToShield{};
  double distanceToLeft{};
  double distanceToRight{};

public:
  void update(const Shield &shield)
  {
    position.x += velocity.dx;
    position.y += velocity.dy;

    if (position.x >= UniverseWidth || position.x < 0) {
      velocity.dx = -velocity.dx;
      position.x += velocity.dx;
    }
    if (position.y >= UniverseHeight || position.y < 0) {
      velocity.dy = -velocity.dy;
      position.y += velocity.dy;
    }

    if (shield.isNear(position, distanceToShield, distanceToLeft, distanceToRight)) {
      velocity.dx = -velocity.dx;
      velocity.dy = -velocity.dy;
      position.x += velocity.dx;
      position.y += velocity.dy;
    }
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockCircle(static_cast<int>(position.x), static_cast<int>(position.y), AsteroidRadius);
  }
};

}// namespace atw
