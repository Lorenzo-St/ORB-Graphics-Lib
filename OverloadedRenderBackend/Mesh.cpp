#include "pch.h"
#include "Mesh.h"
#include "Wermal Reader.h"
#include "Stream.h"
ORB_Mesh::~ORB_Mesh()
{

}




void ORB_Mesh::Read(std::string file)
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

void ORB_Mesh::Read(Stream& file)
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
        //std::cout << v << std::endl;
        _verticies.push_back(v);
      }
      else
        break;
    }

  }
  break;
  }
}


glm::vec4 const& ORB_Mesh::Color() const
{
  return _color;
}

glm::vec4& ORB_Mesh::Color()
{
  return _color;
}

std::vector<Vertex> const& ORB_Mesh::Verticies() const
{
  return _verticies;
}

GLuint& ORB_Mesh::DrawMode()
{
  return _drawMode;
}

GLuint ORB_Mesh::DrawMode() const
{
  return _drawMode;
}

void ORB_Mesh::AddVertex(Vertex const& v)
{
  _verticies.push_back(v);
}
