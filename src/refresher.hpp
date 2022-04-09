
#pragma once

#include <ftxui/component/captured_mouse.hpp>// for ftxui
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <atomic>
#include <thread>

namespace atw {

class Refresher
{
  ftxui::ScreenInteractive &screen;
  std::atomic<bool> stop_thread{};
  std::thread thread{};

public:
  Refresher(ftxui::ScreenInteractive &scr, std::chrono::milliseconds refreshInterval)
    : screen{ scr }, thread{ [this, refreshInterval] {
        while (!stop_thread) {
          std::this_thread::sleep_for(refreshInterval);
          screen.PostEvent(ftxui::Event::Custom);
        }
      } }
  {}

  Refresher(const Refresher &) = delete;
  Refresher &operator=(const Refresher &) = delete;

  ~Refresher()
  {
    stop_thread = true;
    thread.join();
  }
};

}// namespace atw
