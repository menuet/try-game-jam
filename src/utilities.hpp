
#pragma once

#include <cmath>

namespace atw {

struct Point
{
  double x;
  double y;
};

struct Offset
{
  double dx;
  double dy;
};

inline Point rotate(Point p, double angle) noexcept
{
  const auto co = std::cos(angle);
  const auto si = std::sin(angle);
  return { p.x * co - p.y * si, p.x * si + p.y * co };
}

inline Point translate(Point p, Offset offset) noexcept { return { p.x + offset.dx, p.y + offset.dy }; }

inline auto distance(Point p1, Point p2) noexcept
{
  const auto diffX = p2.x - p1.x;
  const auto diffY = p2.y - p1.y;
  const auto dist = std::sqrt(diffX * diffX + diffY * diffY);
  return dist;
}

}
