find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)

# Generic test that uses conan libs
add_executable(intro main.cpp "turn_based.cpp" "turn_based.hpp" "loop_based.cpp" "loop_based.hpp" "snake.cpp" "snake.hpp" "brick.cpp" "brick.hpp")
target_link_libraries(
  intro
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog)

target_link_system_libraries(
  intro
  PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component)

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
