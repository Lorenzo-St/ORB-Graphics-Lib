#include "pch.h"
#include "TexturedMesh.h"
#include "RenderBackend.h"
#include "Wermal Reader.h"
extern Renderer* active;

 TexturedMesh::~TexturedMesh()
{
  TextureManager::Instance()->DropTexture(t);
}

 void TexturedMesh::LoadTexture(const char* path)
{
  t = TextureManager::Instance()->LoadTexture(path);
}

 void TexturedMesh::LoadTexture(std::string path)
{
  LoadTexture(path.c_str());
}

void TexturedMesh::SetTexture(ORB_Texture* _t)
{
  t = _t;
}

void TexturedMesh::Read(std::string file)
{
  Stream s(file);
  fileTypes type = GetFileType(file.substr(file.rfind('.')));
  switch (type)
  {
  case fileTypes::dat:
    std::string token = makeLowerCase(s.readString());
    if (token != "<texturedmesh>")
      return;
    break;
  }
  TexturedMesh::Read(s);
}

void TexturedMesh::Read(Stream& file)
{
  fileTypes type = GetFileType(file.Path().substr(file.Path().rfind('.')));
  switch (type)
  {
  case fileTypes::dat:
  {
    auto p = ReadNextAttribute(file);
    if (p.first == false)
      break;
    std::string path(p.second.get());
    while (path[0] == ' ')
      path.erase(path.begin());
    t = TextureManager::Instance()->LoadTexture(path);
  }
  break;
  }
  this->ORB_Mesh::Read(file);
}

void TexturedMesh::Execute() const
{
  active->SetActiveTexture(t);
}
