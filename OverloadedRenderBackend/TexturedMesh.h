#pragma once
#include "Mesh.h"
#include "Textures.h"
struct TexturedMesh : Mesh 
{
public:
  ~TexturedMesh();
  
  void LoadTexture(const char* path);
  void LoadTexture(std::string path);
  
  void SetTexture(Texture* t);

  void Read(std::string) override;
  void Read(Stream& s) override;

  void Execute() const override;

private:
  Texture* t;
};
