
#include "../src/utilities.hpp"
#include <catch2/catch.hpp>
#include <numbers>

unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}

TEST_CASE("rotate pi", "[utilities]")
{
  // ARRANGE
  const atw::Point p{ 12.3, -45.6 };

  // ACT
  const auto p2 = atw::rotate(p, std::numbers::pi);

  // ASSERT
  REQUIRE(p2.x == Approx(-12.3));
  REQUIRE(p2.y == Approx(45.6));
}

TEST_CASE("rotate pi/2", "[utilities]")
{
  // ARRANGE
  const atw::Point p{ 12.3, -45.6 };

  // ACT
  const auto p2 = atw::rotate(p, std::numbers::pi / 2);

  // ASSERT
  REQUIRE(p2.x == Approx(45.6));
  REQUIRE(p2.y == Approx(12.3));
}

TEST_CASE("distance point/point", "[utilities]")
{
  // ARRANGE
  const atw::Point p1{ 2, 3 };
  const atw::Point p2{ 5, 6 };

  // ACT
  const auto dist = atw::distance(p1, p2);

  // ASSERT
  const auto expectedDist = std::sqrt(3 * 3 + 3 * 3);
  REQUIRE(dist == Approx(expectedDist));
}

TEST_CASE("distance point/line", "[utilities]")
{
  // ARRANGE
  const atw::Point p{ 5, 2 };
  const atw::Segment s{ { 0, -3 }, { 0, 10 } };

  // ACT
  const auto dist = atw::distance(p, s);

  // ASSERT
  const auto expectedDist = 5.0;
  REQUIRE(dist == Approx(expectedDist));
}
