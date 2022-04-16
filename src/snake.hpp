#pragma once

#include <vector>

namespace stella {
struct Point
{
  int x;
  int y;
};

struct Candy
{
  Point center;
};

struct Snake
{
  std::vector<Point> points;
};
struct World 
{
    Candy candy;
    Snake snake;
    int width;
    int heitght;

};

}// namespace stella
