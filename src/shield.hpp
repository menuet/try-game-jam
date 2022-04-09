
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
  Segment segment{ transpose(ShieldLeft, CenterOffset), transpose(ShieldRight, CenterOffset) };

public:
  void update(const Point &mouse)
  {
    angle = std::atan2(mouse.y - CenterOffset.dy, mouse.x - CenterOffset.dx) + std::numbers::pi / 2;
    segment = Segment{ transpose(rotate(ShieldLeft, angle), CenterOffset),
      transpose(rotate(ShieldRight, angle), CenterOffset) };
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockLine(static_cast<int>(segment.p1.x),
      static_cast<int>(segment.p1.y),
      static_cast<int>(segment.p2.x),
      static_cast<int>(segment.p2.y),
      ftxui::Color::DarkOrange);
  }

  bool isNear(const Point &point) const
  {
    const auto distanceToShield = distance(point, segment);
    if (distanceToShield > AsteroidRadius) return false;

    const auto distanceToP1 = distance(point, segment.p1);
    if (distanceToP1 > ShieldSpan * 2) return false;

    const auto distanceToP2 = distance(point, segment.p2);
    if (distanceToP2 > ShieldSpan * 2) return false;

    return true;
  }

  // For unit tests only
  double getAngle() const { return angle; }
  const Segment &getSegment() const { return segment; }
};

}// namespace atw
