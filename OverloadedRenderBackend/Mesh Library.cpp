#include "pch.h"
#include "Mesh Library.h"
#include "Mesh.h"
#include "TexturedMesh.h"

MeshLibrary* MeshLibrary::Instance()
{
  if (_instance == nullptr)
    _instance = new MeshLibrary();
  return _instance;
}

Mesh* MeshLibrary::CreateMesh()
{
  Mesh* m = new Mesh();
  _meshes.push_back(m);
  return m;
}

Mesh* MeshLibrary::CreateMesh(std::string s)
{
  Mesh* m = new Mesh();
  m->Read(s);
  _meshes.push_back(m);
  return m;
}

Mesh* MeshLibrary::CreateMesh(const char* c)
{
  Mesh* m = new Mesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

Mesh* MeshLibrary::CreateTexMesh()
{
  Mesh* m = new TexturedMesh();
  _meshes.push_back(m);
  return m;
}

Mesh* MeshLibrary::CreateTexMesh(std::string c)
{
  Mesh* m = new TexturedMesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

Mesh* MeshLibrary::CreateTexMesh(const char* c)
{
  Mesh* m = new TexturedMesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

void MeshLibrary::DropMesh(Mesh* m)
{
  auto l = std::find(_meshes.begin(), _meshes.end(), m);
  _meshes.erase(l);
  delete m;
}


MeshLibrary::~MeshLibrary()
{
  for (auto m : _meshes) 
  {
    delete m;
  }
  _meshes.clear();
}
