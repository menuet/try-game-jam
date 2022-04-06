#include <array>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <spdlog/spdlog.h>

// This file will be generated automatically when you run the CMake
// configuration step. It creates a namespace called `myproject`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

namespace {

using namespace std::chrono_literals;

struct Point
{
  double x;
  double y;
};

struct Offset
{
  double dx;
  double dy;
};

static constexpr double MicrosSecondsPerSecond = 1'000'000.0;
static constexpr int UniverseWidth = 200;
static constexpr int UniverseHeight = 100;
static constexpr int CenterX = UniverseWidth / 2;
static constexpr int CenterY = UniverseHeight / 2;
static constexpr int EarthRadius = 10;
static constexpr int ShieldRadius = 20;
static constexpr int ShieldSpan = 10;
static constexpr int MouseRatioX = 2;
static constexpr int MouseRatioY = 4;
static constexpr Point ShieldLeft{ -ShieldSpan, -ShieldRadius };
static constexpr Point ShieldRight{ +ShieldSpan, -ShieldRadius };
static constexpr Offset CenterOffset{ CenterX, CenterY };
static constexpr int AsteroidRadius = 2;
static constexpr auto FrameInterval = 50ms;

Point rotate(Point p, double angle) noexcept
{
  const auto co = std::cos(angle);
  const auto si = std::sin(angle);
  return { p.x * co - p.y * si, p.x * si + p.y * co };
}

Point translate(Point p, Offset offset) noexcept { return { p.x + offset.dx, p.y + offset.dy }; }

class Shield
{
  double angle = 0.0;
  Point left{ translate(ShieldLeft, CenterOffset) };
  Point right{ translate(ShieldRight, CenterOffset) };

public:
  void update(Point mouse)
  {
    angle = std::atan2(mouse.y - CenterOffset.dy, mouse.x - CenterOffset.dx);
    left = translate(rotate(ShieldLeft, angle), CenterOffset);
    right = translate(rotate(ShieldRight, angle), CenterOffset);
  }

  void draw(ftxui::Canvas &canvas) const { canvas.DrawBlockLine((int)left.x, (int)left.y, (int)right.x, (int)right.y); }

  bool isNear(const Point &point, double& distanceToShield, double& distanceToLeft, double& distanceToRight) const
  {
    const auto m = (right.y - left.y) / (right.x - left.x);
    const auto num = std::abs(-m * point.x + 1 * point.y - left.y + m * left.x);
    const auto den = std::sqrt(-m * -m + 1 * 1);
    distanceToShield = num / den;

    distanceToLeft = std::sqrt((point.x - left.x) * (point.x - left.x) + (point.y - left.y) * (point.y - left.y));

    distanceToRight = std::sqrt((point.x - right.x) * (point.x - right.x) + (point.y - right.y) * (point.y - right.y));

    if (distanceToShield > 2.0) return false;
    if (distanceToLeft > ShieldSpan * 2) return false;
    if (distanceToRight > ShieldSpan * 2) return false;

    return true;
  }
};

struct Asteroid
{
  Point position{ .x = 20, .y = 0. };
  Offset velocity{ .dx = 1.0, .dy = 1.0 };
  double distanceToShield{};
  double distanceToLeft{};
  double distanceToRight{};

  void update(const Shield &shield)
  {
    position.x += velocity.dx;
    position.y += velocity.dy;

    if (position.x >= UniverseWidth || position.x < 0) {
      velocity.dx = -velocity.dx;
      position.x += velocity.dx;
    }
    if (position.y >= UniverseHeight || position.y < 0) {
      velocity.dy = -velocity.dy;
      position.y += velocity.dy;
    }

    if (shield.isNear(position, distanceToShield, distanceToLeft, distanceToRight)) {
      velocity.dx = -velocity.dx;
      velocity.dy = -velocity.dy;
      position.x += velocity.dx;
      position.y += velocity.dy;
    }
  }

  void draw(ftxui::Canvas &canvas) const { canvas.DrawBlockCircle((int)position.x, (int)position.y, AsteroidRadius); }
};

class GameState
{
  double fps = 0;
  int counter = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_asteroid_time = last_time;
  Point mouse{ CenterX, CenterY };
  Shield shield{};
  std::vector<Asteroid> asteroids{ {} };
  bool pause{};

public:
  void onEvent(ftxui::Event &e)
  {
    if (e.is_mouse()) {
      pause = (e.mouse().button & ftxui::Mouse::Left) != 0;
      mouse.x = e.mouse().x * MouseRatioX;
      mouse.y = e.mouse().y * MouseRatioY;
    }
  }

  void update()
  {
    if (pause) return;
    ++counter;
    const auto new_time = std::chrono::steady_clock::now();
    const auto elapsed_time = new_time - last_time;
    last_time = new_time;
    fps = 1.0
          / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
             / MicrosSecondsPerSecond);
    shield.update(mouse);
    if (new_time - last_asteroid_time < FrameInterval) return;
    last_asteroid_time = new_time;
    for (auto &asteroid : asteroids) asteroid.update(shield);
  }

  auto draw() const
  {
    auto universeComponent = ftxui::Renderer([&] {
      auto canvas = ftxui::Canvas(UniverseWidth, UniverseHeight);
      canvas.DrawBlockCircleFilled(CenterX, CenterY, EarthRadius);
      canvas.DrawPointCircle(CenterX, CenterY, ShieldRadius);
      shield.draw(canvas);
      for (const auto &asteroid : asteroids) asteroid.draw(canvas);
      return ftxui::canvas(std::move(canvas));
    });

    return ftxui::hbox({ universeComponent->Render() | ftxui::borderDouble,
      ftxui::separator(),
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)),
        ftxui::text("FPS: " + std::to_string(fps)),
        ftxui::text("MX: " + std::to_string(mouse.x)),
        ftxui::text("MY: " + std::to_string(mouse.y)),
        ftxui::text("DtoS: " + std::to_string(asteroids[0].distanceToShield)),
        ftxui::text("DtoL: " + std::to_string(asteroids[0].distanceToLeft)),
        ftxui::text("DtoR: " + std::to_string(asteroids[0].distanceToRight)) }) });
  }
};

}// namespace

void brick()
{
  GameState state{};

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  auto renderer = ftxui::Renderer([&]() {
    state.update();
    return state.draw();
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    state.onEvent(e);
    return false;
  });

  std::atomic<bool> refresh_ui_continue = true;

  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      std::this_thread::sleep_for(FrameInterval);// NOLINT magic numbers
      screen.PostEvent(ftxui::Event::Custom);
    }
  });

  screen.Loop(events_catcher);

  refresh_ui_continue = false;
  refresh_ui.join();
}
