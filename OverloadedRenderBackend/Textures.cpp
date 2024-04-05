/*********************************************************************
 * @file   Textures.cpp
 * @brief  Holds texture class implementation
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * 
 * @date   September 2023
 *********************************************************************/

#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Textures.h"
#include "stb_image.h"
#include <iostream>
#include <stacktrace>
extern std::ofstream traceLog;

template <typename Arg, typename... vArgs>
void TextureManager::Log(TraceLevels l, Arg&& arg1, vArgs&&... variadic)
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm ltime;
    localtime_s(&ltime, &t);
    std::stringstream tm;
    tm << std::put_time(&ltime, "%H:%M:%S");
    if (l >= _globalTraceLevel)
    {
        std::cout << tm.str() << "\t" << toString(l) << "\t" << std::forward<Arg>(arg1);
        ((std::cout << " " << std::forward<vArgs>(variadic)), ...);
        std::cout << std::endl;
        traceLog << tm.str() << "\t" << toString(l) << "\t" << std::forward<Arg>(arg1);
        ((traceLog << " " << std::forward<vArgs>(variadic)), ...);
        traceLog << std::endl;

        if (l > TraceLevels::Normal)
            traceLog << std::stacktrace::current() << std::endl;
    }
}

ORB_Texture* TextureManager::LoadTexture(std::string filename, bool KeepAlive)
{
    for (auto& t : _textures)
    {
        if (t->name() == filename)
        {
            t->_uses++;
            return t;
        }
    }
    int w, h, channels;

    unsigned char* file = stbi_load(filename.c_str(), &w, &h, &channels, 4);
    if (file == nullptr)
      return nullptr;
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, file);
    ORB_Texture* t = new ORB_Texture(texture, w, h, GL_RGBA32I, KeepAlive);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if (t == nullptr)
      return nullptr;
    t->name(filename);
    _textures.push_back(t);
    stbi_image_free(file);
    return t;
}

ORB_Texture* TextureManager::LoadTexture(const char* filename)
{
    std::string s = std::string(filename);
    return LoadTexture(s);
}

ORB_Texture* TextureManager::CreateFromMemeory(std::string name, int w, int h, int depth, void* data)
{
    for (auto& t : _textures)
    {
        if (t->name() == name)
        {
            t->_uses++;
            return t;
        }
    }
    // CheckError(__LINE__);
    if (data == 0 || w == 0 || h == 0 || depth == 0)
        return nullptr;
    GLuint texture = 0;
    ORB_Texture* t = nullptr;
    glGenTextures(1, &texture);
    // CheckError(__LINE__);
    glBindTexture(GL_TEXTURE_2D, texture);
    // CheckError(__LINE__);
    switch (depth)
    {
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        // CheckError(__LINE__);
        t = new ORB_Texture(texture, w, h, GL_RGBA32I, false);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // CheckError(__LINE__);
        t = new ORB_Texture(texture, w, h, GL_RGB32I, false);
        break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->name(name);
    _textures.push_back(t);
    return t;
}

void TextureManager::DeleteTextureFromMemory(ORB_Texture* t)
{
    Image i = t->texture();
    glDeleteTextures(1, &i);
    delete t;
}

void TextureManager::Update(void)
{
    for (int i = 0; i < _textures.size(); ++i)
    {
        if (_textures[i]->_keepAlive == false && _textures[i]->_uses-- < -50)
            DropTexture(_textures[i]);
    }
}

void TextureManager::DropTexture(ORB_Texture* ti)
{
    int idx = 0;
    for (auto& t : _textures)
    {
        if (t == ti)
        {
            Log(TraceLevels::High, "Dropped unused Texture: ", t->name());
            Image im = t->texture();
            glDeleteTextures(1, &im);
            delete t;
            t = nullptr;
            _textures.erase(_textures.begin() + idx);
            break;
        }
        ++idx;
    }
}

void TextureManager::DropAll(void)
{
    for (auto& texture : _textures)
    {
        Image im = texture->texture();
        glDeleteTextures(1, &im);
        delete texture;
    }
    _textures.clear();
}

std::vector<ORB_Texture*> const& TextureManager::GetTextures() const
{
    return _textures;
}

TextureManager* TextureManager::Instance()
{
  if (_instance == nullptr)
    return _instance = new TextureManager(), _instance;
  return _instance;
}

TextureManager::TextureManager()
{
    // Clear it to ensure no un initialized values?
    _textures = std::vector<ORB_Texture*>();
    _textures.clear();
}

TextureManager::~TextureManager()
{
    DropAll();
}

__declspec(noinline) void TextureManager::checkError()
{
    int i = glGetError();
    if (i != 0)
    {
        std::string s = "OpenGL error: " + std::to_string(i);
        Log(Error, s);
        traceLog.close();
        exit(-1);
    }
}

Image const ORB_Texture::texture() const
{
  const_cast<ORB_Texture*>(this)->IncramentUses();
    return _texture;
}

std::string const& ORB_Texture::name() const
{
    return _name;
}

void ORB_Texture::name(std::string const& s)
{
    _name = s;
}

int ORB_Texture::Width() const
{
    return _w;
}

int ORB_Texture::Height() const
{
    return _h;
}

GLenum ORB_Texture::Format() const
{
    return _format;
}

void ORB_Texture::IncramentUses()
{
    ++_uses;
}

void ORB_Texture::SetSampleMode(int mode)
{
  glBindTexture(GL_TEXTURE_2D, _texture);
  switch (mode) 
  {
  case 0:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    break;
  case 1:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    break;
  }

}
