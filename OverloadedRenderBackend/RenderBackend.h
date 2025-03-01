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
#include "Mesh.h"

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

typedef struct fboinfo {
  uint fbo;
  uint texture;
}fboinfo;

class Renderer
{
public:
  bool running = true;

  Renderer();
  Renderer(std::string path);
  Renderer(const char* path);

  void EnableDebugOutput(bool b);

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
  void DrawMesh(ORB_Mesh const & v, uint depth);
  void DrawIndexed(ORB_Mesh const & v, int count);

  void SetColor(glm::vec4 const& color);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale, float rot);
  void SetMatrix(glm::vec3 const& pos, glm::vec3 const& scale, glm::vec3 const& rot);
  void SetMatrix(glm::mat4 const& matrix);

  void EnableLighting(bool value);
  void EnableShadows(bool b);
  void EnableStoredRender(bool value);
  void SetLight(glm::vec4 pos, glm::vec3 color);
  void SetMaterial(glm::vec3, glm::vec3, float);
  void SetMaterial(int id);
  glm::vec2 GetWindowSize(Window* w);

  void Update();

  void SetProjectionMode(int);

  void SetActiveTexture(ORB_Texture* t);
  
  void BindBuffer(std::string buffer);
  void UnbindBuffer(std::string buffer);
  void WriteBuffer(std::string buffer, size_t dataSize, void* data);
  void WriteSubBufferData(std::string, int index, size_t structSize, void* data);
  void SetBufferBase(std::string buffer, int base);
  void WriteUniform(std::string buffer, void* data);
  void DispatchCompute(int x, int y, int z);
  void WriteRenderConstantsHere();

  void SetBindings(GLuint b, GLuint VAO);

  glm::vec2 ToWorldSpace(glm::vec2);
  glm::vec2 ToScreenSpace(glm::vec2);

  void SetActiveFont(FontInfo const*);

  void SetBlendMode(int);

  void BindTextureToUnit(ORB_Texture* tex, int texture);
  void BindTextureToUnit(uint tex, int unit);
  fboinfo GetFBOByName(std::string&);
  void BindActiveFBO(fboinfo f);
  void ClearFBO(fboinfo f);

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

  bool Stored() const {return storedRender;}
  struct MaterialInfo {
    glm::vec3 diff;
    float buffer;
    glm::vec3 spec;
    float specExp;
  };
  std::vector<MaterialInfo> _materials;

  glm::mat4& projecton() {
    return _storedProjection;
  }
  bool GetLightingEnabled() {
    return enableLighting;
  }

  glm::mat4 _uiProjection = glm::identity < glm::mat4 >();

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

  bool enableLighting = false;
  bool storedRender = false;
  bool _enableShadows = false;
  bool custom = false;

  RenderInformation _currentObject;


};
