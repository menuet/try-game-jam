
#pragma once

#include "configuration.hpp"
#include "shield.hpp"
#include "utilities.hpp"

namespace atw {

struct Asteroid
{
  Point position{};
  Offset velocity{};
  bool red{};

public:
  void update(const Shield &shield)
  {
    red = !red;
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

    if (shield.isNear(position)) {
      velocity.dx = -velocity.dx;
      velocity.dy = -velocity.dy;
      position.x += velocity.dx;
      position.y += velocity.dy;
    }
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockCircle(static_cast<int>(position.x),
      static_cast<int>(position.y),
      AsteroidRadius,
      red ? ftxui::Color::Red : ftxui::Color::Yellow);
  }
};

}// namespace atw
