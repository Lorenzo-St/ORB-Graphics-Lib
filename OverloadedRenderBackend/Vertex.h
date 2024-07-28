#pragma once
#include <iostream>
#include <format>
#pragma pack(8)
typedef struct Vertex
{
  glm::vec4 pos;
  glm::vec4 color;
  glm::vec4 normal;
  glm::vec2 tex;
}Vertex;


std::ostream& operator<<(std::ostream& os, Vertex const& v);
