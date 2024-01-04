#pragma once
#include <vector>
#include <string>
struct Mesh;

class MeshLibrary 
{
public:

  ~MeshLibrary();
  static MeshLibrary* Instance();
  
  Mesh* CreateMesh();
  Mesh* CreateMesh(std::string);
  Mesh* CreateMesh(const char*);

  Mesh* CreateTexMesh();
  Mesh* CreateTexMesh(std::string);
  Mesh* CreateTexMesh(const char*);
  
  void DropMesh(Mesh*);

private:
  MeshLibrary() = default;
  
  MeshLibrary(MeshLibrary const&) = delete;
  MeshLibrary& operator=(MeshLibrary const&) = delete;
  MeshLibrary(MeshLibrary&&) = delete;


  static inline MeshLibrary* _instance = nullptr;
  std::vector<Mesh*> _meshes;
};
