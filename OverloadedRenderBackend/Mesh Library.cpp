#include "pch.h"
#include "Mesh Library.h"
#include "Mesh.h"
#include "TexturedMesh.h"

MeshLibrary *MeshLibrary::Instance()
{
  if (_instance == nullptr)
    _instance = new MeshLibrary();
  return _instance;
}

ORB_Mesh *MeshLibrary::CreateMesh()
{
  ORB_Mesh *m = new ORB_Mesh();
  _meshes.push_back(m);
  return m;
}

ORB_Mesh *MeshLibrary::CreateMesh(std::string s)
{
  ORB_Mesh* m = new ORB_Mesh();
  auto lam = [s](ORB_Mesh* m) {return m->path == s; };
  auto exists = std::find_if(_meshes.begin(), _meshes.end(), lam);
  if (exists != _meshes.end()) return *exists;
  m->Read(s);
  m->path = s;
  _meshes.push_back(m);
  return m;
}

ORB_Mesh *MeshLibrary::CreateMesh(const char *c)
{
  ORB_Mesh *m = new ORB_Mesh();
  auto lam = [c](ORB_Mesh* m) {return m->path == std::string(c); };
  auto exists = std::find_if(_meshes.begin(), _meshes.end(), lam);
  if (exists != _meshes.end()) return *exists;
  m->Read(c);
  m->path = c;
  _meshes.push_back(m);
  return m;
}

ORB_Mesh *MeshLibrary::CreateTexMesh()
{
  ORB_Mesh *m = new TexturedMesh();
  _meshes.push_back(m);
  return m;
}

ORB_Mesh *MeshLibrary::CreateTexMesh(std::string c)
{
  ORB_Mesh *m = new TexturedMesh();
  m->Read(c);
  m->path = c;
  _meshes.push_back(m);
  return m;
}

ORB_Mesh *MeshLibrary::CreateTexMesh(const char *c)
{
  ORB_Mesh *m = new TexturedMesh();
  m->Read(c);
  m->path = c;
  _meshes.push_back(m);
  return m;
}

void MeshLibrary::DropMesh(ORB_Mesh *m)
{
  auto l = std::find(_meshes.begin(), _meshes.end(), m);
  _meshes.erase(l);
  delete m;
}

ORB_Mesh *MeshLibrary::Find(std::string s)
{
  auto res = std::find_if(_meshes.begin(), _meshes.end(), [&s](ORB_Mesh *m)
                          { return m->path == s; });
  if (res != _meshes.end())
    return *res;
  return nullptr;
}

MeshLibrary::~MeshLibrary()
{
  for (auto m : _meshes)
  {
    delete m;
  }
  _meshes.clear();
}
