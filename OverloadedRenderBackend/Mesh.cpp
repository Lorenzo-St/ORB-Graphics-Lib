#include "pch.h"
#include "Mesh.h"
#include "Wermal Reader.h"
#include "Stream.h"
#include "RenderBackend.h"
#include <exception>
Renderer *ORB_Mesh::_backend = nullptr;
ORB_Mesh::~ORB_Mesh()
{
  glDeleteBuffers(1, &_buffer);
  glDeleteVertexArrays(1, &_vao);
}

ORB_Mesh::ORB_Mesh(int mode, std::vector<Vertex> &verts, glm::vec4 &&col)
    : _drawMode(mode), _buffer(0), _verticies(verts), _color(col)
{
  CreateBuffer();
}

void ORB_Mesh::Read(std::string file)
{
  Stream s(file);
  fileTypes type = GetFileType(file.substr(file.rfind('.')));
  switch (type)
  {
  case fileTypes::dat:
  {
    std::string token = makeLowerCase(s.readString());
    if (token != "<mesh>")
      return;
  }
  break;
  default:
    throw std::runtime_error("File types not handled yet");
    break;
  }
  Read(s);
}

void ORB_Mesh::Read(Stream &file)
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
        v = *reinterpret_cast<Vertex *>(Parse<float>(d.second).get());
        // std::cout << v << std::endl;
        _verticies.push_back(v);
      }
      else
        break;
    }
  }
  break;
  default:
    throw std::runtime_error("File types not handled yet");
    break;
  }

  CalculateNormals();
  CreateBuffer();
  // glWriteBuffer("VBO", v.size() * sizeof(Vertex), (void*)v.data());
}

glm::vec4 const &ORB_Mesh::Color() const
{
  return _color;
}

glm::vec4 &ORB_Mesh::Color()
{
  return _color;
}

std::vector<Vertex> const &ORB_Mesh::Verticies() const
{
  return _verticies;
}

GLuint &ORB_Mesh::DrawMode()
{
  return _drawMode;
}

GLuint ORB_Mesh::Buffer() const
{
  return _buffer;
}

GLuint ORB_Mesh::VAO() const
{
  return _vao;
}

GLuint ORB_Mesh::Size() const
{
  return static_cast<GLuint>(_verticies.size());
}

std::ostream &operator<<(std::ostream &os, glm::vec4 const &p)
{
  os << p.x << " " << p.y << " " << p.z << " " << p.w;
  return os;
}
std::ostream &operator<<(std::ostream &os, glm::vec2 const &p)
{
  os << p.x << " " << p.y;
  return os;
}
void ORB_Mesh::Dump() const
{
  for (auto &v : _verticies)
  {
    std::cout << "Pos: " << v.pos << " Color: " << v.color << " Normal: " << v.normal << " UV: " << v.tex << std::endl;
  }
}

void ORB_Mesh::EndMesh()
{
  if (_buffer == 0b11111111111111111111111111111111)
  {
    CalculateNormals();
    CreateBuffer();
  }
}
void ORB_Mesh::Render()
{
  _backend->WriteBuffer("RenderBuffer", sizeof(RenderInformation) * _renderCalls.size(), _renderCalls.data());
  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  glDrawArraysInstanced(_drawMode, 0, _verticies.size(), _renderCalls.size());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void ORB_Mesh::Reset() 
{
  _renderCalls.clear();
}
void ORB_Mesh::AddCall(glm::mat4 matrix, glm::vec3 color, int matID) {
  _renderCalls.push_back({.matrix = matrix, .normalMatrix = glm::inverseTranspose(matrix), .color = color, .materialID = matID});
}
void ORB_Mesh::AddCall(RenderInformation const &r)
{
  _renderCalls.push_back(r);
}
void CheckError(int);
void ORB_Mesh::CreateBuffer()
{
  if (_backend->QueryAndSet("primary") || _backend->QueryAndSet("default"))
  {
    glCreateBuffers(1, &_buffer);
    glGenVertexArrays(1, &_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, _verticies.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
    // std::cout << offsetof(Vertex, pos) << " " << offsetof(Vertex, color) << " " << offsetof(Vertex, normal) << " " << offsetof(Vertex, tex) << std::endl;
#ifndef __CLANG
    if constexpr (std::endian::native == std::endian::little)
    {
      glBufferData(GL_ARRAY_BUFFER, _verticies.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
      for (int i = 0; i < _verticies.size(); i++)
      {
        Vertex &v = _verticies[i];
        glBufferSubData(GL_ARRAY_BUFFER, (i * sizeof(Vertex)) + 0, sizeof(Vertex::tex), &(v.tex));
        glBufferSubData(GL_ARRAY_BUFFER, (i * sizeof(Vertex)) + 0 + sizeof(glm::vec2), sizeof(Vertex::normal), &(v.normal));
        glBufferSubData(GL_ARRAY_BUFFER, (i * sizeof(Vertex)) + 0 + sizeof(glm::vec2) + sizeof(glm::vec4), sizeof(Vertex::color), &(v.normal));
        glBufferSubData(GL_ARRAY_BUFFER, (i * sizeof(Vertex)) + 0 + sizeof(glm::vec2) + (sizeof(glm::vec4) * 2), sizeof(Vertex::pos), &(v.pos));
      }
    }
    else if constexpr (std::endian::native == std::endian::big)
    {
#endif
      glBufferData(GL_ARRAY_BUFFER, _verticies.size() * sizeof(Vertex), _verticies.data(), GL_STATIC_DRAW);
#ifndef __CLANG
    }
#endif
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    _backend->SetBindings(_buffer, _vao);
  }
  else
  {
    abort();
  }
}

void ORB_Mesh::CalculateNormals()
{
  switch (_drawMode)
  {
  case GL_TRIANGLES:
    for (int i = 0; i < _verticies.size() - 2; i += 3)
    {
      Vertex &a = _verticies[i];
      Vertex &b = _verticies[i + 1];
      Vertex &c = _verticies[i + 2];

      glm::vec3 ab = b.pos - a.pos;
      glm::vec3 ac = c.pos - a.pos;
      glm::vec4 normal = glm::vec4(glm::normalize(glm::cross(ab, ac)), 0);
      a.normal = normal;
      b.normal = normal;
      c.normal = normal;
    }
    break;
  case GL_TRIANGLE_STRIP:

    break;
  case GL_TRIANGLE_FAN:
    Vertex &first = _verticies[0];
    for (int i = 1; i < _verticies.size() - 1; i += 1)
    {
    }
    break;
  }
}

GLuint ORB_Mesh::DrawMode() const
{
  return _drawMode;
}

void ORB_Mesh::AddVertex(Vertex const &v)
{
  _verticies.push_back(v);
}
