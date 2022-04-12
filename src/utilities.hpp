
#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <string>

namespace atw {

struct Point
{
  double x{};
  double y{};
};

struct Segment
{
  Point p1{};
  Point p2{};
};

struct Offset
{
  double dx{};
  double dy{};
};

constexpr inline Point transpose(Point p, Offset offset) noexcept { return { p.x + offset.dx, p.y + offset.dy }; }

inline Point rotate(Point p, double angle) noexcept
{
  const auto co = std::cos(angle);
  const auto si = std::sin(angle);
  return { p.x * co - p.y * si, p.x * si + p.y * co };
}

inline auto distance(Point p1, Point p2) noexcept
{
  const auto diffX = p2.x - p1.x;
  const auto diffY = p2.y - p1.y;
  const auto dist = std::sqrt(diffX * diffX + diffY * diffY);
  return dist;
}

inline auto distance(Point p, Segment s) noexcept
{
  if (s.p2.x != s.p1.x) {
    const auto m = (s.p2.y - s.p1.y) / (s.p2.x - s.p1.x);
    const auto num = std::abs(-m * p.x + 1 * p.y - s.p1.y + m * s.p1.x);
    const auto den = std::sqrt(-m * -m + 1 * 1);
    return num / den;
  }
  return std::abs(p.x - s.p1.x);
}

inline auto &randomGenerator()
{
  static std::random_device rd{};
  static std::mt19937 gen{ rd() };
  return gen;
}

inline int randomNumber(int min, int max)
{
  std::uniform_int_distribution<> distrib(min, max);
  return distrib(randomGenerator());
}

inline double randomNumber(double min, double max)
{
  std::uniform_real_distribution<> distrib(min, max);
  return distrib(randomGenerator());
}

inline std::string stretchText(std::size_t desiredLength, const std::string &text)
{
  std::string stretchedText{};
  if (text.length() < desiredLength) {
    const auto desiredGrowth = desiredLength - text.length();
    const auto existingSpacesCount = static_cast<unsigned>(std::count(begin(text), end(text), ' '));
    const auto insertedSpacesCountPerExistingSpace = desiredGrowth / existingSpacesCount;
    for (const auto c : text) {
      stretchedText += c;
      if (c == ' ') {
        for (std::size_t i = 0; i < insertedSpacesCountPerExistingSpace; ++i) stretchedText += c;
      }
    }
    return stretchedText;
  }
  return text;
}

}// namespace atw
