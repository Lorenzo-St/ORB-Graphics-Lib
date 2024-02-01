#pragma once
#include <vector>
#include <string>
struct ORB_Mesh;

class MeshLibrary 
{
public:

  ~MeshLibrary();
  static MeshLibrary* Instance();
  
  ORB_Mesh* CreateMesh();
  ORB_Mesh* CreateMesh(std::string);
  ORB_Mesh* CreateMesh(const char*);

  ORB_Mesh* CreateTexMesh();
  ORB_Mesh* CreateTexMesh(std::string);
  ORB_Mesh* CreateTexMesh(const char*);
  
  void DropMesh(ORB_Mesh*);

private:
  MeshLibrary() = default;
  
  MeshLibrary(MeshLibrary const&) = delete;
  MeshLibrary& operator=(MeshLibrary const&) = delete;
  MeshLibrary(MeshLibrary&&) = delete;


  static inline MeshLibrary* _instance = nullptr;
  std::vector<ORB_Mesh*> _meshes;
};
