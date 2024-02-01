#pragma once
#include <iostream>
#include <format>
typedef struct Vertex
{
  glm::vec4 pos;
  glm::vec4 color;
  glm::vec2 tex;
}Vertex;


std::ostream& operator<<(std::ostream& os, Vertex const& v);
