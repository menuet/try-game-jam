
#include "../src/universe.hpp"
#include "../src/utilities.hpp"
#include <catch2/catch.hpp>
#include <chrono>
#include <numbers>

using namespace std::chrono_literals;

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
  static constexpr atw::Point p{ 12.3, -45.6 };

  // ACT
  const auto result = atw::rotate(p, std::numbers::pi);

  // ASSERT
  static constexpr atw::Point expected{ -12.3, 45.6 };
  REQUIRE(result.x == Approx(expected.x));
  REQUIRE(result.y == Approx(expected.y));
}

TEST_CASE("rotate pi/2", "[utilities]")
{
  // ARRANGE
  static constexpr atw::Point p{ 12.3, -45.6 };

  // ACT
  const auto result = atw::rotate(p, std::numbers::pi / 2);

  // ASSERT
  static constexpr atw::Point expected{ 45.6, 12.3 };
  REQUIRE(result.x == Approx(expected.x));
  REQUIRE(result.y == Approx(expected.y));
}

TEST_CASE("distance point/point", "[utilities]")
{
  // ARRANGE
  static constexpr atw::Point p1{ 2, 3 };
  static constexpr atw::Point p2{ 5, 6 };

  // ACT
  const auto result = atw::distance(p1, p2);

  // ASSERT
  const auto expected = std::sqrt(3 * 3 + 3 * 3);
  REQUIRE(result == Approx(expected));
}

TEST_CASE("distance point/line", "[utilities]")
{
  // ARRANGE
  static constexpr atw::Point p{ 5, 2 };
  static constexpr atw::Segment s{ { 0, -3 }, { 0, 10 } };

  // ACT
  const auto result = atw::distance(p, s);

  // ASSERT
  const auto expected = 5.0;
  REQUIRE(result == Approx(expected));
}

TEST_CASE("universe constructor", "[universe]")
{
  // ARRANGE
  static constexpr auto time = std::chrono::steady_clock::time_point{ 1ms };
  double index = 0.0;
  const auto generateAsteroid = [&] {
    ++index;
    return atw::Asteroid{ { index, index }, { index, index } };
  };

  // ACT
  const atw::Universe universe{ time, generateAsteroid };

  // ASSERT
  REQUIRE(universe.getPoints() == 0);
  REQUIRE(universe.getAsteroids().size() == atw::InitialAsteroidsCount);
}

TEST_CASE("universe update before asteroid creation interval", "[universe]")
{
  // ARRANGE
  static constexpr auto time = std::chrono::steady_clock::time_point{ 0ms };
  double index = 0.0;
  const auto generateAsteroid = [&] {
    ++index;
    return atw::Asteroid{ { index, index }, { index, index } };
  };
  atw::Universe universe{ time, generateAsteroid };

  // ACT
  universe.update(time + atw::AsteroidCreationInterval / 2, {});

  // ASSERT
  REQUIRE(universe.getPoints() == 0);
  REQUIRE(universe.getAsteroids().size() == atw::InitialAsteroidsCount);
}

TEST_CASE("universe update after asteroid creation interval", "[universe]")
{
  // ARRANGE
  static constexpr auto time = std::chrono::steady_clock::time_point{ 0ms };
  double index = 0.0;
  const auto generateAsteroid = [&] {
    ++index;
    return atw::Asteroid{ { index, index }, { index, index } };
  };
  atw::Universe universe{ time, generateAsteroid };

  // ACT
  universe.update(time + 3 * atw::AsteroidCreationInterval / 2, {});

  // ASSERT
  REQUIRE(universe.getPoints() == 0);
  REQUIRE(universe.getAsteroids().size() == atw::InitialAsteroidsCount + 1);
}

TEST_CASE("shield onEvent", "[shield]")
{
  // ARRANGE
  static constexpr auto mouse = atw::transpose(atw::EarthCenter, { 0.0, -50.0 });
  atw::Shield shield{};

  // ACT
  shield.update(mouse);

  // ASSERT
  const auto expectedAngle = 0.0;
  REQUIRE(shield.getAngle() == Approx(expectedAngle));
  const auto expectedSegment =
    atw::Segment{ { atw::EarthCenter.x - atw::ShieldSpan, atw::EarthCenter.y - atw::ShieldRadius },
      {
        atw::EarthCenter.x + atw::ShieldSpan,
        atw::EarthCenter.y - atw::ShieldRadius,
      } };
  REQUIRE(shield.getSegment().p1.x == Approx(expectedSegment.p1.x));
  REQUIRE(shield.getSegment().p1.y == Approx(expectedSegment.p1.y));
  REQUIRE(shield.getSegment().p2.x == Approx(expectedSegment.p2.x));
  REQUIRE(shield.getSegment().p2.y == Approx(expectedSegment.p2.y));
}

TEST_CASE("asteroid is near Earth", "[asteroid]")
{
  // ARRANGE
  static constexpr auto p = atw::transpose(atw::EarthCenter, { 0.0, atw::EarthRadius });
  static constexpr auto v = atw::Offset{ 2.0, 3.0 };
  atw::Asteroid asteroid{ p, v };

  // ACT
  const auto result = asteroid.isNearEarth();

  // ASSERT
  REQUIRE(result);
}

TEST_CASE("asteroid is not near Earth", "[asteroid]")
{
  // ARRANGE
  static constexpr auto p = atw::transpose(atw::EarthCenter, { 0.0, atw::EarthRadius + atw::AsteroidRadius + 1.0 });
  static constexpr auto v = atw::Offset{ 2.0, 3.0 };
  atw::Asteroid asteroid{ p, v };

  // ACT
  const auto result = asteroid.isNearEarth();

  // ASSERT
  REQUIRE_FALSE(result);
}

TEST_CASE("earth update and survive", "[earth]")
{
  // ARRANGE
  const std::vector<atw::Asteroid> asteroids{
    atw::Asteroid{ { 0., 0. }, {} },
    atw::Asteroid{ { 0., atw::UniverseHeight }, {} },
    atw::Asteroid{ { atw::UniverseWidth, 0 }, {} },
    atw::Asteroid{ { atw::UniverseWidth, atw::UniverseHeight }, {} },
  };
  atw::Earth earth{};

  // ACT
  const auto result = earth.update(asteroids);

  // ASSERT
  REQUIRE(result);
}

TEST_CASE("earth update and die", "[earth]")
{
  // ARRANGE
  const std::vector<atw::Asteroid> asteroids{
    atw::Asteroid{ { 0., 0. }, {} },
    atw::Asteroid{ { 0., atw::UniverseHeight }, {} },
    atw::Asteroid{ { atw::UniverseWidth, 0. }, {} },
    atw::Asteroid{ { atw::UniverseWidth, atw::UniverseHeight }, {} },
    atw::Asteroid{ { atw::EarthCenter.x + atw::EarthRadius / 2.0, atw::EarthCenter.y - atw::EarthRadius / 2.0 }, {} },
  };
  atw::Earth earth{};

  // ACT
  const auto result = earth.update(asteroids);

  // ASSERT
  REQUIRE_FALSE(result);
}
