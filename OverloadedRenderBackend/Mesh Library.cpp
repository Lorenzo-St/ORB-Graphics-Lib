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

ORB_Mesh* MeshLibrary::CreateMesh()
{
  ORB_Mesh* m = new ORB_Mesh();
  _meshes.push_back(m);
  return m;
}

ORB_Mesh* MeshLibrary::CreateMesh(std::string s)
{
  ORB_Mesh* m = new ORB_Mesh();
  m->Read(s);
  _meshes.push_back(m);
  return m;
}

ORB_Mesh* MeshLibrary::CreateMesh(const char* c)
{
  ORB_Mesh* m = new ORB_Mesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

ORB_Mesh* MeshLibrary::CreateTexMesh()
{
  ORB_Mesh* m = new TexturedMesh();
  _meshes.push_back(m);
  return m;
}

ORB_Mesh* MeshLibrary::CreateTexMesh(std::string c)
{
  ORB_Mesh* m = new TexturedMesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

ORB_Mesh* MeshLibrary::CreateTexMesh(const char* c)
{
  ORB_Mesh* m = new TexturedMesh();
  m->Read(c);
  _meshes.push_back(m);
  return m;
}

void MeshLibrary::DropMesh(ORB_Mesh* m)
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
