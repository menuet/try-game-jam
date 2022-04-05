#include "ftxui/component/component.hpp"// for Renderer, CatchEvent, Horizontal, Menu, Tab
#include "ftxui/component/component_base.hpp"// for ComponentBase
#include "ftxui/component/event.hpp"// for Event
#include "ftxui/component/mouse.hpp"// for Mouse
#include "ftxui/component/screen_interactive.hpp"// for ScreenInteractive
#include "ftxui/dom/canvas.hpp"// for Canvas
#include "ftxui/screen/color.hpp"// for Color, Color::Red, Color::Blue, Color::Green, ftxui
#include <cmath>// for sin, cos
#include <ftxui/dom/elements.hpp>// for canvas, Element, separator, hbox, operator|, border
#include <ftxui/screen/screen.hpp>// for Pixel
#include <memory>// for allocator, shared_ptr, __shared_ptr_access
#include <string>// for string, basic_string
#include <utility>// for move
#include <vector>// for vector, __alloc_traits<>::value_type

int snake()
{
  using namespace ftxui;

  static constexpr int Width = 300;
  static constexpr int Height = 200;

  int mouse_x = 0;
  int mouse_y = 0;

  // An ellipse following the mouse filled, using block characters.
  auto renderer_ellipse_filled_block = Renderer([&] {
    auto c = Canvas(Width, Height);
    c.DrawText(0, 0, "A filled ellipse (block)");
    c.DrawBlockEllipseFilled(mouse_x / 2, mouse_y / 2, mouse_x / 2, mouse_y / 2);
    c.DrawBlockEllipse(mouse_x / 2, mouse_y / 2, mouse_x / 2, mouse_y / 2);
    return canvas(std::move(c));
  });

  // This capture the last mouse position.
  auto tab_with_mouse = CatchEvent(renderer_ellipse_filled_block, [&](Event e) {
    if (e.is_mouse()) {
      mouse_x = (e.mouse().x - 1) * 2;
      mouse_y = (e.mouse().y - 1) * 4;
    }
    return false;
  });

  int selected_tab = 0;
  std::vector<std::string> tab_titles = {
    "ellipse filled (block)",
  };
  auto tab_toggle = Menu(&tab_titles, &selected_tab);

  auto component = Container::Horizontal({
    tab_with_mouse,
    tab_toggle,
  });

  // Add some separator to decorate the whole component:
  auto component_renderer = Renderer(component, [&] {
    return hbox({
             tab_with_mouse->Render(),
             separator(),
             tab_toggle->Render(),
           })
           | border;
  });

  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(component_renderer);

  return 0;
}
