
#pragma once

#include "configuration.hpp"
#include "earth.hpp"
#include "satellite.hpp"
#include "shield.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <functional>

namespace atw {

enum class State {
  Intro = 1,
  Play = 2,
  End = 3,
};

enum class EventType {
  Unknown,
  Frame,
  Start,
  Mouse,
  Left,
  Right,
};

struct Event
{
  EventType type{};
  Point mouse{};
};

class Universe
{
  std::size_t points{};
  std::chrono::steady_clock::time_point lastSatelliteCreationTime{};
  Earth earth{};
  Shield shield{};
  std::vector<Satellite> satellites{};
  std::function<Satellite()> createSatellite{};
  State state{ State::Intro };
  std::vector<std::string> introText{
    "ONCE UPON A TIME, IN A NEAR NEAR GALAXY,",
    "CHANCELLOR PALPUTIN HAS TAKEN CONTROL",
    "OVER ALL SATELLITES OF THE STARLUKE COMPANY.",
    "THEY THREATEN TO CRASH ON EARTH AND DESTROY IT.",
    "ELONMUSH, A SMART JEDI, IMAGINES A DESPERATE DEFENSE:",
    "TO MANEUVER THE INTERNATIONAL SPACE STATION",
    "AND USE IT AS A SHIELD TO PROTECT THE EARTH.",
    "ALAS, HE IS NOT BOLD ENOUGH TO DO IT BY HIMSELF.",
    ". . .",
    "BUT, YOU, O YOUNG PADAWAN, WILL YOU TAKE UP THE CHALLENGE",
    "AND DRIVE THE ISS AND TRY RESISTING AS LONG AS YOU CAN?!",
    ". . .",
    "TYPE [RETURN] TO START THE GAME, THEN MOVE THE MOUSE OR TYPE [RIGHT]/[LEFT] TO MOVE THE ISS AROUND THE EARTH",
  };
  int introTextOffset{ UniverseHeight - CharHeight * 2 };
  std::chrono::steady_clock::time_point lastIntroTextScrollTime{};

public:
  explicit Universe(std::chrono::steady_clock::time_point now, std::function<Satellite()> satelliteCreator)
    : lastSatelliteCreationTime{ now }, createSatellite{ std::move(satelliteCreator) }, lastIntroTextScrollTime{ now }
  {
    satellites.resize(InitialSatellitesCount);
    std::generate(begin(satellites), end(satellites), createSatellite);
  }

  void update(std::chrono::steady_clock::time_point now, const Event &e)
  {
    switch (state) {
    case State::Intro:
      updateIntro(now, e);
      break;
    case State::Play:
      updateGame(now, e);
      break;
    case State::End:
    default:
      break;
    }
  }

  void updateIntro(std::chrono::steady_clock::time_point now, const Event &e)
  {
    switch (e.type) {
    case EventType::Start:
      state = State::Play;
      break;
    case EventType::Frame:
      if (now - lastIntroTextScrollTime >= IntroTextScrollInterval) {
        lastIntroTextScrollTime = now;
        introTextOffset = (introTextOffset >= CharHeight) ? introTextOffset - CharHeight : 0;
      }
      break;
    default:
      break;
    }
  }

  void updateGame(std::chrono::steady_clock::time_point now, const Event &e)
  {
    switch (e.type) {
    case EventType::Mouse:
      shield.update(e.mouse);
      break;
    case EventType::Left:
      shield.rotateLeft();
      break;
    case EventType::Right:
      shield.rotateRight();
      break;
    case EventType::Frame:
      for (auto &satellite : satellites)
        if (satellite.update(shield)) points += satellites.size();
      if (!earth.update(satellites)) {
        state = State::End;
      } else if (now - lastSatelliteCreationTime >= SatelliteCreationInterval) {
        lastSatelliteCreationTime = now;
        satellites.push_back(createSatellite());
      }
      break;
    default:
      break;
    }
  }

  auto draw() const
  {
    auto universeComponent = ftxui::Renderer([&] {
      auto canvas = ftxui::Canvas(UniverseWidth, UniverseHeight);
      if (state == State::Intro)
        drawIntro(canvas);
      else
        drawGame(canvas);
      return ftxui::canvas(std::move(canvas));
    });

    return ftxui::hbox({ universeComponent->Render() | ftxui::borderDouble,
      ftxui::separator(),
      ftxui::vbox({ ftxui::text("Satellites: " + std::to_string(satellites.size())),
        ftxui::text("Points: " + std::to_string(points)) }) });
  }

  void drawGame(ftxui::Canvas &canvas) const
  {
    earth.draw(canvas);
    shield.draw(canvas);
    for (const auto &satellite : satellites) satellite.draw(canvas);
  }

  void drawIntro(ftxui::Canvas &canvas) const
  {
    int lineIndex = 0;
    for (const auto &line : introText) {
      ++lineIndex;
      drawIntroLine(canvas, lineIndex, line);
    }
  }

  void drawIntroLine(ftxui::Canvas &canvas, int lineIndex, const std::string &line) const
  {
    const auto lineY = introTextOffset + lineIndex * CharHeight * 2;
    const auto offsetY = (UniverseHeight - lineY) * 2;
    const auto desiredWidth = UniverseWidth - offsetY;
    const auto desiredLength = desiredWidth / CharWidth;
    const auto stretchedLine = stretchText(static_cast<std::size_t>(desiredLength), line);
    const auto lineSize = stretchedLine.length() * CharWidth;
    const auto remainingSize = UniverseWidth > lineSize ? UniverseWidth - lineSize : 0;
    const auto lineX = static_cast<int>(remainingSize / 2);
    canvas.DrawText(lineX, lineY, stretchedLine, ftxui::Color::BlueLight);
  }

  // For unit tests only
  std::size_t getPoints() const { return points; }
  const Shield &getShield() const { return shield; }
  const std::vector<Satellite> &getSatellites() const { return satellites; }
  State getState() const { return state; }
  int getIntroTextOffset() const { return introTextOffset; }
};

}// namespace atw
