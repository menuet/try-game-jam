
#include "../src/utilities.hpp"
#include <catch2/catch.hpp>

constexpr unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed with constexpr", "[factorial]")
{
  STATIC_REQUIRE(Factorial(1) == 1);
  STATIC_REQUIRE(Factorial(2) == 2);
  STATIC_REQUIRE(Factorial(3) == 6);
  STATIC_REQUIRE(Factorial(10) == 3628800);
}

TEST_CASE("transpose", "[utilities]")
{
  // ARRANGE
  static constexpr atw::Point p{ 12.3, -45.6 };
  static constexpr atw::Offset o{ -7.8, 9.1 };

  // ACT
  static constexpr auto p2 = atw::transpose(p, o);

  // ASSERT
  REQUIRE(p2.x == Approx(4.5));
  REQUIRE(p2.y == Approx(-36.5));
}
