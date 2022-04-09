
#pragma once

#include "utilities.hpp"
#include <chrono>
#include <numbers>

namespace atw {

using namespace std::chrono_literals;

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
static constexpr double ShieldAngleStep = std::numbers::pi / 16.0;
static constexpr Offset CenterOffset{ EarthCenter.x, EarthCenter.y };
static constexpr std::size_t InitialSatellitesCount = 3;
static constexpr int SatelliteRadius = 2;
static constexpr auto SatelliteCreationInterval = 5s;
static constexpr double SatelliteMinSpeed = 1.0;
static constexpr double SatelliteMaxSpeed = 2.5;
static constexpr auto FrameInterval = 50ms;
static constexpr auto IntroTextScrollInterval = 1s;
static constexpr int CharWidth = 2;
static constexpr int CharHeight = 4;

}// namespace atw
