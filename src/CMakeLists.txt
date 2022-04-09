find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)

# Generic test that uses conan libs
add_executable(
  aroundtheworld
  main.cpp
  aroundtheworld.cpp
  aroundtheworld.hpp
  utilities.hpp
  configuration.hpp
  shield.hpp
  asteroid.hpp
  earth.hpp
  universe.hpp)
target_link_libraries(
  aroundtheworld
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog)

target_link_system_libraries(
  aroundtheworld
  PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
  smk::smk)

target_include_directories(aroundtheworld PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
