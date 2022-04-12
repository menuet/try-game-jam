
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
  static constexpr auto time = std::chrono::steady_clock::time_point{ 0ms };
  double index = 0.0;
  const auto generateSatellite = [&] {
    ++index;
    return atw::Satellite{ { index, index }, { index, index } };
  };

  // ACT
  const atw::Universe universe{ time, generateSatellite };

  // ASSERT
  REQUIRE(universe.getPoints() == 0);
  REQUIRE(universe.getSatellites().size() == atw::InitialSatellitesCount);
  REQUIRE(universe.getState() == atw::State::Intro);
}

#ifdef _MSC_VER

// I believe that the warning readability-function-cognitive-complexity should be disabled in tests
// in order to allow nested TEST_CASEs / SECTIONs style
// But I don't know how to do it.
// I tried : #pragma clang diagnostic ignored "-Wreadability-function-cognitive-complexity"
// I tried : target_compile_options(tests PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-readability-function-cognitive-complexity>)
// But did not succeed.
// So I disable the entire test except with Microsoft's compiler

TEST_CASE("universe update", "[universe]")
{
  // ARRANGE
  static constexpr auto time = std::chrono::steady_clock::time_point{ 0ms };
  double index = 0.0;
  const auto generateSatellite = [&] {
    ++index;
    return atw::Satellite{ { index, index }, { index, index } };
  };
  atw::Universe universe{ time, generateSatellite };

  SECTION("update Intro with Frame event after a too small delay")
  {
    // ARRANGE
    const auto previousOffset = universe.getIntroTextOffset();

    // ACT
    universe.update(time + atw::IntroTextScrollInterval - 1ms, { atw::EventType::Frame });

    // ASSERT
    REQUIRE(universe.getState() == atw::State::Intro);
    REQUIRE(universe.getIntroTextOffset() == previousOffset);
  }

  SECTION("update Intro with Frame event after enough delay")
  {
    // ARRANGE
    const auto previousOffset = universe.getIntroTextOffset();

    // ACT
    universe.update(time + atw::IntroTextScrollInterval, { atw::EventType::Frame });

    // ASSERT
    REQUIRE(universe.getState() == atw::State::Intro);
    REQUIRE(universe.getIntroTextOffset() == previousOffset - atw::CharHeight);
  }

  SECTION("update Intro with Frame events after full scroll")
  {
    // ARRANGE
    static constexpr int LotsOfFrameEvents = 5000;

    // ACT
    for (int i = 0; i < LotsOfFrameEvents; ++i) {
      universe.update(time + i * atw::IntroTextScrollInterval, { atw::EventType::Frame });
    }

    // ASSERT
    REQUIRE(universe.getState() == atw::State::Intro);
    REQUIRE(universe.getIntroTextOffset() == 0);
  }

  SECTION("update Intro with Start event")
  {
    // ARRANGE
    const auto previousOffset = universe.getIntroTextOffset();

    // ACT
    universe.update(time + atw::IntroTextScrollInterval - 1ms, { atw::EventType::Start });

    // ASSERT
    REQUIRE(universe.getState() == atw::State::Play);
    REQUIRE(universe.getIntroTextOffset() == previousOffset);

    SECTION("update Game with Mouse event")
    {
      // ARRANGE
      static constexpr int MouseOffsetX = 50;
      static constexpr int MouseOffsetY = 0;
      static constexpr double ExpectedAngle = std::numbers::pi / 2.0;

      // ACT
      universe.update(time + atw::IntroTextScrollInterval,
        { atw::EventType::Mouse, { .x = atw::EarthCenter.x + MouseOffsetX, .y = atw::EarthCenter.y + MouseOffsetY } });

      // ASSERT
      REQUIRE(universe.getState() == atw::State::Play);
      REQUIRE(universe.getShield().getAngle() == Approx(ExpectedAngle));
      REQUIRE(universe.getPoints() == 0);
      REQUIRE(universe.getSatellites().size() == atw::InitialSatellitesCount);
    }

    SECTION("update Game with Left event")
    {
      // ACT
      universe.update(time + atw::IntroTextScrollInterval, { atw::EventType::Left });

      // ASSERT
      REQUIRE(universe.getState() == atw::State::Play);
      REQUIRE(universe.getShield().getAngle() == Approx(0. - atw::ShieldAngleStep));
      REQUIRE(universe.getPoints() == 0);
      REQUIRE(universe.getSatellites().size() == atw::InitialSatellitesCount);
    }

    SECTION("update Game with Right event")
    {
      // ACT
      universe.update(time + atw::IntroTextScrollInterval, { atw::EventType::Right });

      // ASSERT
      REQUIRE(universe.getState() == atw::State::Play);
      REQUIRE(universe.getShield().getAngle() == Approx(0. + atw::ShieldAngleStep));
      REQUIRE(universe.getPoints() == 0);
      REQUIRE(universe.getSatellites().size() == atw::InitialSatellitesCount);
    }

    SECTION("update Game with Frame event")
    {
      // ACT
      universe.update(time + atw::IntroTextScrollInterval, { atw::EventType::Frame });

      // ASSERT
      REQUIRE(universe.getState() == atw::State::Play);
      REQUIRE(universe.getShield().getAngle() == Approx(0.));
      REQUIRE(universe.getPoints() == 0);
      REQUIRE(universe.getSatellites().size() == atw::InitialSatellitesCount);
    }
  }
}

#endif // _MSC_VER

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

TEST_CASE("satellite is near Earth", "[satellite]")
{
  // ARRANGE
  static constexpr auto p = atw::transpose(atw::EarthCenter, { 0.0, atw::EarthRadius });
  static constexpr auto v = atw::Offset{ 2.0, 3.0 };
  atw::Satellite satellite{ p, v };

  // ACT
  const auto result = satellite.isNearEarth();

  // ASSERT
  REQUIRE(result);
}

TEST_CASE("satellite is not near Earth", "[satellite]")
{
  // ARRANGE
  static constexpr auto p = atw::transpose(atw::EarthCenter, { 0.0, atw::EarthRadius + atw::SatelliteRadius + 1.0 });
  static constexpr auto v = atw::Offset{ 2.0, 3.0 };
  atw::Satellite satellite{ p, v };

  // ACT
  const auto result = satellite.isNearEarth();

  // ASSERT
  REQUIRE_FALSE(result);
}

TEST_CASE("earth update and survive", "[earth]")
{
  // ARRANGE
  const std::vector<atw::Satellite> satellites{
    atw::Satellite{ { 0., 0. }, {} },
    atw::Satellite{ { 0., atw::UniverseHeight }, {} },
    atw::Satellite{ { atw::UniverseWidth, 0 }, {} },
    atw::Satellite{ { atw::UniverseWidth, atw::UniverseHeight }, {} },
  };
  atw::Earth earth{};

  // ACT
  const auto result = earth.update(satellites);

  // ASSERT
  REQUIRE(result);
}

TEST_CASE("earth update and die", "[earth]")
{
  // ARRANGE
  const std::vector<atw::Satellite> satellites{
    atw::Satellite{ { 0., 0. }, {} },
    atw::Satellite{ { 0., atw::UniverseHeight }, {} },
    atw::Satellite{ { atw::UniverseWidth, 0. }, {} },
    atw::Satellite{ { atw::UniverseWidth, atw::UniverseHeight }, {} },
    atw::Satellite{ { atw::EarthCenter.x + atw::EarthRadius / 2.0, atw::EarthCenter.y - atw::EarthRadius / 2.0 }, {} },
  };
  atw::Earth earth{};

  // ACT
  const auto result = earth.update(satellites);

  // ASSERT
  REQUIRE_FALSE(result);
}
