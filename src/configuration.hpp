
#pragma once

#include "utilities.hpp"
#include <chrono>

namespace atw {

using namespace std::chrono_literals;

static constexpr double MicrosSecondsPerSecond = 1'000'000.0;
static constexpr int UniverseWidth = 300;
static constexpr int UniverseHeight = 150;
static constexpr Point EarthCenter{ UniverseWidth / 2, UniverseHeight / 2 };
static constexpr int EarthRadius = 30;
static constexpr int ShieldRadius = EarthRadius + 10;
static constexpr int ShieldSpan = 10;
static constexpr int MouseRatioX = 2;
static constexpr int MouseRatioY = 4;
static constexpr Point ShieldLeft{ -ShieldSpan, -ShieldRadius };
static constexpr Point ShieldRight{ +ShieldSpan, -ShieldRadius };
static constexpr Offset CenterOffset{ EarthCenter.x, EarthCenter.y };
static constexpr std::size_t InitialAsteroidsCount = 3;
static constexpr int AsteroidRadius = 2;
static constexpr auto AsteroidCreationInterval = 5s;
static constexpr double AsteroidMinSpeed = 1.0;
static constexpr double AsteroidMaxSpeed = 3.0;
static constexpr auto FrameInterval = 50ms;

}// namespace atw
