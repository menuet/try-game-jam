#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <numbers>
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
static constexpr int AsteroidRadius = 2;
static constexpr auto FrameInterval = 50ms;

Point rotate(Point p, double angle) noexcept
{
  const auto co = std::cos(angle);
  const auto si = std::sin(angle);
  return { p.x * co - p.y * si, p.x * si + p.y * co };
}

Point translate(Point p, Offset offset) noexcept { return { p.x + offset.dx, p.y + offset.dy }; }

auto distance(Point p1, Point p2) noexcept
{
  const auto diffX = p2.x - p1.x;
  const auto diffY = p2.y - p1.y;
  const auto dist = std::sqrt(diffX * diffX + diffY * diffY);
  return dist;
}

class Shield
{
  double angle = 0.0;
  Point left{ translate(ShieldLeft, CenterOffset) };
  Point right{ translate(ShieldRight, CenterOffset) };

public:
  void update(Point mouse)
  {
    angle = std::atan2(mouse.y - CenterOffset.dy, mouse.x - CenterOffset.dx) + std::numbers::pi / 2;
    left = translate(rotate(ShieldLeft, angle), CenterOffset);
    right = translate(rotate(ShieldRight, angle), CenterOffset);
  }

  void draw(ftxui::Canvas &canvas) const { canvas.DrawBlockLine((int)left.x, (int)left.y, (int)right.x, (int)right.y); }

  bool isNear(const Point &point, double &distanceToShield, double &distanceToLeft, double &distanceToRight) const
  {
    if (right.x != left.x) {
      const auto m = (right.y - left.y) / (right.x - left.x);
      const auto num = std::abs(-m * point.x + 1 * point.y - left.y + m * left.x);
      const auto den = std::sqrt(-m * -m + 1 * 1);
      distanceToShield = num / den;
    } else
      distanceToShield = std::abs(point.x - left.x);

    distanceToLeft = distance(point, left);

    distanceToRight = distance(point, right);

    if (distanceToShield > AsteroidRadius) return false;
    if (distanceToLeft > ShieldSpan * 2) return false;
    if (distanceToRight > ShieldSpan * 2) return false;

    return true;
  }
};

struct Asteroid
{
  Point position{};
  Offset velocity{};
  double distanceToShield{};
  double distanceToLeft{};
  double distanceToRight{};

public:
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

class Earth
{
  bool is_destroyed{};

public:
  bool update(const std::vector<Asteroid> &asteroids)
  {
    is_destroyed = std::any_of(std::begin(asteroids), std::end(asteroids), [](const auto &a) {
      return distance(a.position, EarthCenter) <= EarthRadius + AsteroidRadius;
    });
    return !is_destroyed;
  }

  void draw(ftxui::Canvas &canvas) const
  {
    canvas.DrawBlockCircleFilled((int)EarthCenter.x, (int)EarthCenter.y, EarthRadius);
    canvas.DrawPointCircle((int)EarthCenter.x, (int)EarthCenter.y, ShieldRadius);
    if (is_destroyed)
      canvas.DrawText((int)EarthCenter.x - 20, (int)EarthCenter.y, "BOOOOOOOOOOOOOOOOOOOOOOM", ftxui::Color::Red);
  }
};

class Universe
{
  double fps = 0;
  int counter = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_asteroid_time = last_time;
  Point mouse{};
  Earth earth{};
  Shield shield{};
  std::vector<Asteroid> asteroids{
    { { .x = EarthCenter.x / 2, .y = 0. }, { .dx = 1.0, .dy = 1.0 } },
    { { .x = EarthCenter.x * 3 / 2, .y = UniverseHeight }, { .dx = 1.0, .dy = -1.0 } },
  };

public:
  void onEvent(ftxui::Event &e)
  {
    if (e.is_mouse()) {
      mouse.x = e.mouse().x * MouseRatioX;
      mouse.y = e.mouse().y * MouseRatioY;
    }
  }

  void update()
  {
    if (!earth.update(asteroids)) return;

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
      earth.draw(canvas);
      shield.draw(canvas);
      for (const auto &asteroid : asteroids) asteroid.draw(canvas);
      return ftxui::canvas(std::move(canvas));
    });

    return ftxui::hbox({ universeComponent->Render() | ftxui::borderDouble,
      ftxui::separator(),
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)),
        ftxui::text("FPS: " + std::to_string(fps)) }) });
  }
};

}// namespace

void brick()
{
  Universe universe{};

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  auto renderer = ftxui::Renderer([&]() {
    universe.update();
    return universe.draw();
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    universe.onEvent(e);
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
