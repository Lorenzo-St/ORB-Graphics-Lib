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
typedef struct ORB_Texture Texture;
typedef struct Vertex Vertex;
enum class renderStage;
extern unsigned int _activePolyMode;
typedef struct Window
{
  int x = 0, y = 0, w = 1280, h = 720;
  int vx = 0, vy = 0, vw = w, vh = h;
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

  void SetWindowScale(Window*, int w, int h);
  void SetWindowPosition(Window*, int x, int y);
  void SetWindowViewPort(Window* wi, int x, int y, int w, int h);
  void SetWindowMaximized(Window* w);
  void SetWindowFullScreen(Window* w, int type);


  void SetActiveWindow(Window* w);

  void DestroyWindow(Window* w);

  void LoadRenderPass(std::string path);
  void LoadRenderPass(const char* path);

  void RegisterCallBack(int stage, int id, renderCallBack fn);

  void DrawRect(glm::vec2 pos, glm::vec2 scale, float rot, uint depth = 1);
  void DrawMesh(std::vector<Vertex> const& v, uint depth, int poly = 6);


  void SetColor(glm::vec4 const& color);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale, float rot);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale, glm::vec3 const& rot);
  void SetMatrix(glm::mat4 const& matrix);


  glm::vec2 GetWindowSize(Window* w);

  void Update();

  void SetProjectionMode(int);

  void SetActiveTexture(ORB_Texture* t);
  
  void WriteBuffer(std::string buffer, size_t dataSize, void* data);
  
  void WriteUniform(std::string buffer, void* data);

  void DispatchCompute(int x, int y, int z);

  glm::vec2 ToWorldSpace(glm::vec2);
  glm::vec2 ToScreenSpace(glm::vec2);

  void SetActiveFont(FontInfo const*);

  ORB_Texture* RenderText(const char*, glm::vec4 const&, int);
  FontInfo const* ActiveFont();

  void SetZoom(float f);

  Camera& GetCamera();

  void SetDefualtRenderMode(int i);

  void SetFillMode(int i);

  bool QueryAndSet(std::string name);

  void ResizeFBOs();

  // Zoom
  float  _zoom = 1;

private:

  void UpdateRenderConstants();

  // Projection mode
  int _projection = 0;

  // Projection Matrix
  glm::mat4 _projectionMatrix = glm::identity < glm::mat4 >() ;

  // Stored Projection
  glm::mat4 _storedProjection = glm::identity < glm::mat4 >();


  // Camera
  Camera mainCamera;

  // The active renderpass
  RenderPass* _activePass;

  // The current active window
  Window* _window;


  // Window Size
  glm::vec2 _windowSize;
  
  // ACtive Font
  FontInfo* _activeFont;
  
  // Fill mode
  GLuint renderMode = GL_TRIANGLE_FAN;

};
