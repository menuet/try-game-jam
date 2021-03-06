
#pragma once

#include "configuration.hpp"
#include "shield.hpp"
#include "utilities.hpp"

namespace atw {

inline Point randomPosition()
{
  switch (randomNumber(1, 2)) {
  case 1:
    return { .x = 0.0, .y = randomNumber(0.0, static_cast<double>(UniverseHeight)) };
  case 2:
  default:
    return { .x = UniverseWidth, .y = randomNumber(0.0, static_cast<double>(UniverseHeight)) };
  }
}

inline Offset randomVelocity(double x) noexcept
{
  const auto angle = x == 0.0 ? randomNumber(-std::numbers::pi / 4, std::numbers::pi / 4)
                              : randomNumber(-3 * std::numbers::pi / 4, 3 * std::numbers::pi / 4);
  const auto speed = randomNumber(SatelliteMinSpeed, SatelliteMaxSpeed);
  return { speed * std::cos(angle), speed * std::sin(angle) };
}

class Satellite
{
  Point position{};
  Offset velocity{};
  bool red{};

public:
  Satellite() = default;

  Satellite(Point p, Offset v) : position{ p }, velocity{ v } {}

  bool isNearEarth() const { return distance(position, EarthCenter) <= EarthRadius + SatelliteRadius; }

  bool update(const Shield &shield)
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
      return true;
    }

    return false;
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockCircle(static_cast<int>(position.x),
      static_cast<int>(position.y),
      SatelliteRadius,
      red ? ftxui::Color::Red : ftxui::Color::Yellow);
  }
};

inline Satellite randomSatellite()
{
  auto pos = randomPosition();
  return Satellite(pos, randomVelocity(pos.x));
}


}// namespace atw
