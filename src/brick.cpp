#include <array>
#include <functional>
#include <iostream>
#include <random>

#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <spdlog/spdlog.h>

// This file will be generated automatically when you run the CMake
// configuration step. It creates a namespace called `myproject`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>

namespace {

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
static constexpr int Width = 200;
static constexpr int Height = 100;
static constexpr int CenterX = Width / 2;
static constexpr int CenterY = Height / 2;
static constexpr int EarthRadius = 10;
static constexpr int ShieldRadius = 20;
static constexpr int ShieldSpan = 10;
static constexpr int MouseRatioX = 2;
static constexpr int MouseRatioY = 4;
static constexpr Point ShieldLeft{ -ShieldSpan, -ShieldRadius };
static constexpr Point ShieldRight{ +ShieldSpan, -ShieldRadius };
static constexpr Offset CenterOffset{ CenterX, CenterY };

Point rotate(Point p, double angle) noexcept
{
  const auto co = std::cos(angle);
  const auto si = std::sin(angle);
  return { p.x * co - p.y * si, p.x * si + p.y * co };
}

Point translate(Point p, Offset offset) noexcept { return { p.x + offset.dx, p.y + offset.dy }; }

struct Shield
{
  double angle = 0.0;
  Point left{ translate(ShieldLeft, CenterOffset) };
  Point right{ translate(ShieldRight, CenterOffset) };

  void update(Point mouse)
  {
    angle = std::atan2(mouse.y - CenterOffset.dy, mouse.x - CenterOffset.dx);
    left = translate(rotate(ShieldLeft, angle), CenterOffset);
    right = translate(rotate(ShieldRight, angle), CenterOffset);
  }

  void draw(ftxui::Canvas &canvas) const { canvas.DrawBlockLine((int)left.x, (int)left.y, (int)right.x, (int)right.y); }
};

struct GameState
{
  double fps = 0;
  int counter = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  Point mouse{ CenterX, CenterY };
  Shield shield{};

  void onEvent(ftxui::Event &e)
  {
    if (e.is_mouse()) {
      mouse.x = e.mouse().x * MouseRatioX;
      mouse.y = e.mouse().y * MouseRatioY;
    }
  }

  void update()
  {
    ++counter;
    const auto new_time = std::chrono::steady_clock::now();
    const auto elapsed_time = new_time - last_time;
    last_time = new_time;
    fps = 1.0
                / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
                   / MicrosSecondsPerSecond);
    shield.update(mouse);
  }

  auto draw() const
  {
    auto world = ftxui::Renderer([&] {
      auto canvas = ftxui::Canvas(Width, Height);
      canvas.DrawBlockCircleFilled(CenterX, CenterY, EarthRadius);
      canvas.DrawPointCircle(CenterX, CenterY, ShieldRadius);
      shield.draw(canvas);
      return ftxui::canvas(std::move(canvas));
    });

    return ftxui::hbox({ world->Render() | ftxui::borderDouble,
      ftxui::separator(),
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)),
        ftxui::text("FPS: " + std::to_string(fps)),
        ftxui::text("MX: " + std::to_string(mouse.x)),
        ftxui::text("MY: " + std::to_string(mouse.y)) }) });
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

  using namespace std::chrono_literals;
  static constexpr auto frame_interval = 1.0s;// / 30.0;

  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      std::this_thread::sleep_for(frame_interval);// NOLINT magic numbers
      screen.PostEvent(ftxui::Event::Custom);
    }
  });

  screen.Loop(events_catcher);

  refresh_ui_continue = false;
  refresh_ui.join();
}
