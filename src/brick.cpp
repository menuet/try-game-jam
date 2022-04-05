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

static constexpr int Width = 200;
static constexpr int Height = 100;

struct GameState
{
  double fps = 0;
  int counter = 0;
  std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
  int mouse_x = 40;
  int mouse_y = 20;
};

// to do, add total game time clock also, not just current elapsed time
void update(GameState &state)
{
  ++state.counter;
  const auto new_time = std::chrono::steady_clock::now();
  const auto elapsed_time = new_time - state.last_time;
  state.last_time = new_time;
  state.fps = 1.0
              / (static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count())
                 / 1'000'000.0);// NOLINT magic numbers
}

auto draw(const GameState &state)
{
  auto world = ftxui::Renderer([&] {
    auto c = ftxui::Canvas(Width, Height);
    c.DrawBlockEllipseFilled(Width / 2, Height / 2, state.mouse_x, state.mouse_y);
    return ftxui::canvas(std::move(c));
  });

  return ftxui::hbox({ world->Render() | ftxui::borderDouble,
    ftxui::separator(),
    ftxui::vbox(
      { ftxui::text("Frame: " + std::to_string(state.counter)),
      ftxui::text("FPS: " + std::to_string(state.fps)),
      ftxui::text("MX: " + std::to_string(state.mouse_x)),
      ftxui::text("MY: " + std::to_string(state.mouse_y)) }) });
}

}// namespace

void brick()
{
  GameState state{};

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  auto renderer = ftxui::Renderer([&]() {
    update(state);
    return draw(state);
  });

  auto events_catcher = ftxui::CatchEvent(renderer, [&](ftxui::Event e) {
    if (e.is_mouse()) {
      state.mouse_x = e.mouse().x;
      state.mouse_y = e.mouse().y;
    }
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
