#pragma once
#include "Mesh.h"
#include "Textures.h"
struct TexturedMesh : ORB_Mesh 
{
public:
  ~TexturedMesh();
  
  void LoadTexture(const char* path);
  void LoadTexture(std::string path);
  
  void SetTexture(ORB_Texture* t);

  void Read(std::string) override;
  void Read(Stream& s) override;

  void Execute() const override;

private:
  ORB_Texture* t;
};
