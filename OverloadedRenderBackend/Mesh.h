#pragma once
#include <glm.hpp>
#include <vector>
#include <string>
#include "Stream.h"
#include "Vertex.h"
class Renderer;
typedef struct RenderInformation {

  glm::mat4 matrix;
  glm::mat4 normalMatrix;
  glm::vec3 color;
  // float buffer;
  int materialID  = 0;

}RenderInformation;
struct ORB_Mesh 
{
public:

  virtual ~ORB_Mesh();
  ORB_Mesh(int mode, std::vector<Vertex>& verts, glm::vec4&& col);
  ORB_Mesh() = default;
  virtual void Read(std::string file);
  virtual void Read(Stream& file);
  virtual void Execute() const {};

  glm::vec4 const& Color() const;
  glm::vec4 & Color();

  void AddVertex(Vertex const&);
  std::vector<Vertex> const& Verticies() const;

  GLuint DrawMode() const;
  GLuint& DrawMode();

  GLuint Buffer() const;
  GLuint VAO() const;
  GLuint Size() const;
  void Dump() const;
  void EndMesh();
  void Render();
  void Reset();
  void AddCall(glm::mat4 matrix, glm::vec3 color, int matID);
  void AddCall(RenderInformation const &);

  static Renderer* _backend;

  int renderLayer = 1;
  std::string path;

private:
  void CreateBuffer();
  void CalculateNormals();
  
  GLuint _drawMode = 6;
  GLuint _buffer = 0b11111111111111111111111111111111; // 32 1s, the same as 0xffffffff
  GLuint _vao = 0b11111111111111111111111111111111; // 32 1s, the same as 0xffffffff

  
  std::vector<RenderInformation> _renderCalls;
  std::vector<Vertex> _verticies;
  glm::vec4 _color = {1,1,1,1};
};

