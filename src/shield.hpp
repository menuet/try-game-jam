
#pragma once

#include "configuration.hpp"
#include "utilities.hpp"
#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <numbers>

namespace atw {

class Shield
{
  double angle{};
  Point left{ translate(ShieldLeft, CenterOffset) };
  Point right{ translate(ShieldRight, CenterOffset) };

public:
  void update(Point mouse)
  {
    angle = std::atan2(mouse.y - CenterOffset.dy, mouse.x - CenterOffset.dx) + std::numbers::pi / 2;
    left = translate(rotate(ShieldLeft, angle), CenterOffset);
    right = translate(rotate(ShieldRight, angle), CenterOffset);
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockLine(
      static_cast<int>(left.x), static_cast<int>(left.y), static_cast<int>(right.x), static_cast<int>(right.y));
  }

  bool isNear(const Point &point, double &distanceToShield, double &distanceToLeft, double &distanceToRight) const
  {
    if (right.x != left.x) {
      const auto m = (right.y - left.y) / (right.x - left.x);
      const auto num = std::abs(-m * point.x + 1 * point.y - left.y + m * left.x);
      const auto den = std::sqrt(-m * -m + 1 * 1);
      distanceToShield = num / den;
    } else
      distanceToShield = std::abs(point.x - left.x);

    distanceToLeft = distance(point, left);

    distanceToRight = distance(point, right);

    if (distanceToShield > AsteroidRadius) return false;
    if (distanceToLeft > ShieldSpan * 2) return false;
    if (distanceToRight > ShieldSpan * 2) return false;

    return true;
  }
};

}// namespace atw
