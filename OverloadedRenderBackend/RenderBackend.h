/*********************************************************************
 * @file   RenderBackend.h
 * @brief
 *
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#pragma once
#include <glad.h>
#include <glm.hpp>
#include <SDL.h>
#include <array>
#include "Camera.h"
#include "Fonts.h"

class RenderPass;
typedef int (*renderCallBack)();
typedef unsigned int uint;
typedef struct Texture Texture;
typedef struct Vertex Vertex;
enum class renderStage;

typedef struct Window
{
  int x = 0, y = 0, w = 1280, h = 720;
  bool primary = false;

  std::string name = "";
  std::string VAO = "";
  
  SDL_Window* window = nullptr;
  SDL_GLContext context = nullptr;
  
  union
  {
    glm::vec4 color = {0,0,0,0};
    struct 
    {
      float r, g, b, a;
    };
  };

}Window;

class Renderer
{
public:
  bool running = true;

  Renderer();
  Renderer(std::string path);
  Renderer(const char* path);

  Window* GetWindow();
  Window* MakeWindow(std::string s = "");
  void SetActiveWindow(Window* w);

  void DestroyWindow(Window* w);

  void LoadRenderPass(std::string path);
  void LoadRenderPass(const char* path);

  void RegisterCallBack(int stage, int id, renderCallBack fn);

  void DrawRect(glm::vec2 pos, glm::vec2 scale, float rot, uint depth = 1);
  void DrawMesh(std::vector<Vertex> const& v, uint depth, int poly = 6);


  void SetColor(glm::vec4 color);
  void SetMatrix(glm::vec3 pos, glm::vec3 scale);
  void SetMatrix(glm::vec3 pos, glm::vec3 scale, float rot);
  void SetMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot);

  glm::vec2 GetWindowSize();

  void Update();

  void SetProjectionMode(int);

  void SetActiveTexture(Texture* t);
  
  void WriteBuffer(std::string buffer, size_t dataSize, void* data);
  
  void WriteUniform(std::string buffer, void* data);

  void DispatchCompute(int x, int y, int z);

  glm::vec2 ToWorldSpace(glm::vec2);
  glm::vec2 ToScreenSpace(glm::vec2);

  void SetActiveFont(FontInfo const*);

  Texture* RenderText(const char*, glm::vec4 const&, int);
  FontInfo const* ActiveFont();
private:

  void UpdateRenderConstants();

  // Projection mode
  int _projection = 0;

  // Zoom
  float  _zoom = 1;

  // Camera
  Camera mainCamera;

  // The active renderpass
  RenderPass* _activePass;

  // The current active window
  Window* _window;


  // Window Size
  glm::vec2 _windowSize;

  FontInfo* _activeFont;

};
