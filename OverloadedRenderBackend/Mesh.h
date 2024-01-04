#pragma once
#include <glm.hpp>
#include <vector>
#include <string>
#include "Stream.h"
#include "Vertex.h"

struct Mesh 
{
public:

  virtual ~Mesh();

  virtual void Read(std::string file);
  virtual void Read(Stream& file);
  virtual void Execute() const {};

  glm::vec4 const& Color() const;
  glm::vec4 & Color();

  void AddVertex(Vertex const&);
  std::vector<Vertex> const& Verticies() const;

  GLuint DrawMode() const;
  GLuint& DrawMode();

  
private:
  GLuint _drawMode = 6;
  std::vector<Vertex> _verticies;
  glm::vec4 _color = {1,1,1,1};
};

