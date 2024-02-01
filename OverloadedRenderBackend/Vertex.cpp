#include "pch.h"
#include "Vertex.h"

std::ostream& operator<<(std::ostream& os, Vertex const& v)
{
    os << std::format("Pos: ({}, {}) Color: ({}, {}, {}, {}) Tex: ({}, {})", v.pos.x, v.pos.y, v.color.r, v.color.g, v.color.b, v.color.a, v.tex.x, v.tex.y);
    return os;
}
