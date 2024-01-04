#include "pch.h"
#include "Mesh.h"
#include "Wermal Reader.h"
#include "Stream.h"
Mesh::~Mesh()
{

}




void Mesh::Read(std::string file)
{
  Stream s(file);
  fileTypes type = GetFileType(file.substr(file.rfind('.')));
  switch (type) 
  {
  case fileTypes::dat:
    std::string token = makeLowerCase(s.readString());
    if (token != "<mesh>")
      return;
    break;
  }
  Read(s);
}

void Mesh::Read(Stream& file)
{
  fileTypes type = GetFileType(file.Path().substr(file.Path().rfind('.')));
  switch (type)
  {
  case fileTypes::dat:
  {
    auto p = ReadNextAttribute(file);
    if (p.first == false)
      break;
    _drawMode = *Parse<int>(p.second);
    while (file.isEOF() == false)
    {

      Vertex v = Vertex();
      auto d = ReadNextAttribute(file);
      if (d.first == true && d.second.get()[0] != '\0')
      {
        v = *reinterpret_cast<Vertex*>(Parse<float>(d.second).get());
        _verticies.push_back(v);
      }
      else
        break;
    }

  }
  break;
  }
}


glm::vec4 const& Mesh::Color() const
{
  return _color;
}

glm::vec4& Mesh::Color()
{
  return _color;
}

std::vector<Vertex> const& Mesh::Verticies() const
{
  return _verticies;
}

GLuint& Mesh::DrawMode()
{
  return _drawMode;
}

GLuint Mesh::DrawMode() const
{
  return _drawMode;
}

void Mesh::AddVertex(Vertex const& v)
{
  _verticies.push_back(v);
}
