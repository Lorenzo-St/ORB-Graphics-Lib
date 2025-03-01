/*********************************************************************
 * @file   RenderBackend.cpp
 * @brief
 *
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#include "pch.h"
#include "RenderBackend.h"
#include "RenderPass.h"
#include "Textures.h"
#include "Vertex.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#define LOG_WINDOW_SWAPS 0
#include "Mesh Library.h"
// Used for sending ponter to value containing true or false
const int zero = 0;
const int one = 1;
extern Window *defaultWindow;
static Renderer *local = nullptr;
void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
  std::cout << "---------------------opengl-callback-start------------" << std::endl;
  std::cout << " message: " << message << std::endl;
  std::cout << " type: ";
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "ERROR";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "DEPRECATED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "UNDEFINED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "PORTABILITY";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "PERFORMANCE";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "OTHER";
    break;
  }
  std::cout << std::endl;

  std::cout << " id: " << id << std::endl;
  std::cout << " severity: ";
  switch (severity)
  {
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "LOW";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "MEDIUM";
    break;
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "HIGH";
    break;
  default:
    std::cout << "IRRELEVANT";
    break;
  }
  std::cout << std::endl;
  std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}
extern std::vector<Window *> activeWindows;

void CheckError(int l)
{

#if _DEBUG & 0
  int i = glGetError();
  if (i != 0)
  {
    std::cerr << "Line " << l << " : " << i << std::endl;
    throw i;
  }
#endif
}

void RefreshWindow(Window *w)
{
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.windowID = SDL_GetWindowID(w->window);
  e.window.event = SDL_WINDOWEVENT_RESIZED;
  SDL_PushEvent(&e);
}
Renderer::Renderer()
    : _window(nullptr), _activeFont(nullptr)
{
  SDL_Init(SDL_INIT_VIDEO);
  int n = SDL_GetNumVideoDrivers();
  auto driver = SDL_GetVideoDriver(0);
  int res = SDL_VideoInit(driver);
  assert(res == 0);
  local = this;
  const char *title = "Primary Window";
  int width = 1280, height = 720;

  _windowSize = {width, height};

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  _window = new Window();

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);

  _window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, width, height,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  SDL_GLContext context = SDL_GL_CreateContext(_window->window);

  _window->context = context;
  _window->w = width, _window->h = height;

  int state = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
  if (state == 0)
    abort();
  SDL_GL_MakeCurrent(_window->window, context);
  SDL_GL_SetSwapInterval(1);

  // glClearColor(204.f / 255.f, 154.f / 255.f, 254.f / 255.f, .25f);

  _window->color = {0, 0, 0, 0};

  glClearColor(_window->r, _window->g, _window->b, _window->a);
  glClear(GL_COLOR_BUFFER_BIT);

  SDL_GL_SwapWindow(_window->window);

  glClear(GL_COLOR_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
  glCullFace(GL_BACK);

  _activePass = new RenderPass();
  _window->primary = true;
  _window->name = std::move(title);
  _window->VAO = "VAO";
  std::cout << "Default Window: " << title << std::endl;
}

Renderer::Renderer(std::string path) : Renderer()
{
  LoadRenderPass(path);
}

Renderer::Renderer(const char *path) : Renderer()
{
  LoadRenderPass(path);
}

void Renderer::EnableDebugOutput(bool b)
{
  if (b)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
  }
  else
  {
    glDisable(GL_DEBUG_OUTPUT);
  }
}

Window *Renderer::GetWindow()
{
  return _window;
}

Window *Renderer::MakeWindow(std::string s)
{
  Window *result = new Window();

  std::string title = ((s == "") ? "Secondary Window " + std::to_string(activeWindows.size()) : s);
  int width = 1280, height = 720;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

  result->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, width, height,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  result->context = SDL_GL_CreateContext(_window->window);

  SDL_GL_MakeCurrent(result->window, result->context);
  SDL_GL_SetSwapInterval(1);

  // glClearColor(204.f / 255.f, 154.f / 255.f, 254.f / 255.f, .25f);
  result->color = {0.1f, 0, 0, 0.2f};

  glClearColor(result->r, result->g, result->b, result->a);

  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(result->window);
  glClear(GL_COLOR_BUFFER_BIT);

  result->context = _window->context;
  result->w = width, result->h = height;

  SDL_GL_MakeCurrent(_window->window, _window->context);

  result->primary = false;
  result->name = std::move(title);
  std::cout << "New Window: " << result->name << std::endl;
  return result;
}

void Renderer::SetActiveWindow(Window *w)
{
  //if (w == _window) return;
  if (activeWindows.size() > 1)
    glFlush();
#if LOG_WINDOW_SWAPS
  static int change = 0;
  std::cout << "Changing to window " << w << " Change #" << change++ << std::endl;
#endif
  if (w->context == nullptr || w->window == nullptr)
  {
    if (_window->context == nullptr || w->window == nullptr)
    {
      auto next = std::find_if(activeWindows.begin(), activeWindows.end(), [](Window *w)
                               { return w->context != nullptr && w->window != nullptr; });
      if (next != activeWindows.end())
      {
        _window = *next;
        if (SDL_GL_MakeCurrent(_window->window, _window->context))
        {
          std::cout << SDL_GetError() << std::endl;
          throw;
        }
      }
      else
      {
        running = false;
      }
    }
    return;
  }
  _window = w;
  // auto fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  // GLenum result;
  // while (true)
  //{
  //   result = glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)); //5 Second timeout
  //   if (result != GL_TIMEOUT_EXPIRED) break; //we ignore timeouts and wait until all OpenGL commands are processed!
  // }
  if (SDL_GL_MakeCurrent(_window->window, _window->context) != 0)
    throw "Bad";
  //assert(_window->window == SDL_GL_GetCurrentWindow() && "Window swap successful");
  UpdateRenderConstants();
}
void Renderer::SetWindowMaximized(Window *w)
{
  SDL_MaximizeWindow(w->window);
  RefreshWindow(w);
}
void Renderer::SetWindowFullScreen(Window *w, int type)
{
  switch (type)
  {
  case 0:
    SDL_SetWindowFullscreen(w->window, 0);

    break;
  case 1:
    SDL_SetWindowFullscreen(w->window, SDL_WINDOW_FULLSCREEN);
    break;
  case 2:
    SDL_SetWindowFullscreen(w->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    break;
  }
  RefreshWindow(w);
}
void Renderer::DestroyWindow(Window *w)
{
  // if (w == defaultWindow)
  // running = false;
  SDL_DestroyWindow(w->window);
  auto res = std::find(activeWindows.begin(), activeWindows.end(), w);
  activeWindows.erase(res);
  w->window = nullptr;
  w->context = nullptr;
}

void Renderer::SetWindowViewPort(Window *wi, int x, int y, int w, int h)
{
  if (w == 0 || h == 0)
    return;
  wi->vx = x, wi->vy = y, wi->vw = w, wi->vh = h;
}

void Renderer::SetWindowScale(Window *wi, int w, int h)
{
  if (w == 0 || h == 0)
    return;
  SDL_SetWindowSize(wi->window, w, h);
  RefreshWindow(wi);
}

void Renderer::SetWindowPosition(Window *w, int x, int y)
{
  SDL_SetWindowPosition(w->window, x, y);
  RefreshWindow(w);
}

void Renderer::LoadRenderPass(std::string path)
{
  LoadRenderPass(path.c_str());
}

void Renderer::LoadRenderPass(const char *path)
{
  local = this;
  custom = true;
  // TODO: Make this check for API version

  if (_activePass != nullptr)
    delete _activePass;

  _activePass = new RenderPass(path);
  for (auto &w : activeWindows)
  {
    w->VAO = "";
  }
}

void Renderer::WriteBuffer(std::string buffer, size_t dataSize, void *data)
{
  _activePass->BindBuffer(buffer);
  _activePass->WriteBuffer(buffer, dataSize, data);
  _activePass->UnBindBuffer(buffer);
}

void Renderer::WriteUniform(std::string uniform, void *data)
{
  _activePass->WriteAttribute(uniform, data);
}

glm::vec2 Renderer::ToWorldSpace(glm::vec2 src)
{

  auto screenSize = glm::vec2(_window->w, _window->h);
  return ((src * glm::vec2(1, -1)) + glm::vec2(-(screenSize.x / 2.0f), (screenSize.y / 2.0f))) * (1.0f / _zoom) - glm::vec2(mainCamera.Position());
}

void Renderer::SetBufferBase(std::string buffer, int base)
{
  _activePass->SetBufferBase(buffer, base);
}

void Renderer::WriteSubBufferData(std::string s, int index, size_t structSize, void *data)
{
  _activePass->BindBuffer(s);
  _activePass->WriteSubBufferData(s, index, structSize, data);
  _activePass->UnBindBuffer(s);
}

void Renderer::DispatchCompute(int x, int y, int z)
{
  _activePass->DispatchCompute(x, y, z);
}

void Renderer::WriteRenderConstantsHere()
{
  enum projtype
  {
    ortho,
    perspective,
  };
  if (_activePass->QuerryAttribute("screenMatrix") == false)
  {
    std::cerr << "ORB ERROR: Main rendering stage must have mat4 named 'screenMatrix'. This matrix will be used for camera and projection" << std::endl;
    throw std::invalid_argument("ORB ERROR : Main rendering stage must have mat4 names 'screenMatrix'.This matrix will be used for camera and projection");
  }

  if (_activePass->QuerryAttribute("zoom") == false)
  {
    std::cerr << "ORB ERROR: Main rendering stage must have float named 'zooom'. This matrix will be used for proper zooming of camera" << std::endl;
    throw std::invalid_argument("ORB ERROR: Main rendering stage must have float named 'zooom'. This matrix will be used for proper zooming of camera");
  }

  glm::mat4 camMat = mainCamera.GetMatrix();
  switch (_projection)
  {
  case ortho:
    _projectionMatrix = glm::ortho(
        -_windowSize.x / 2.0f,
        _windowSize.x / 2.0f,
        -_windowSize.y / 2.0f,
        _windowSize.y / 2.0f,
        0.f,
        1000.f);
    _storedProjection = _projectionMatrix * camMat;
    break;
  case perspective:
    glm::vec3 camPos = camMat * glm::vec4(0, 0, 0, 1);
    glm::vec3 lookatPos = camMat * glm::vec4(0, 0, -1, 1);
    glm::mat4 view = glm::lookAt(camPos, lookatPos, mainCamera.UpVector());
    glm::mat4 perp = glm::perspective(120.0f, _windowSize.x / _windowSize.y, 1.0f, 1500.0f);
    _projectionMatrix = perp * view;
    _storedProjection = _projectionMatrix /** camMat*/;
    //_storedProjection[3][3] = 1;
    break;
  }
  _activePass->WriteAttribute("screenMatrix", &_storedProjection[0][0]);
  _activePass->WriteAttribute("zoom", &_zoom);
  if (_activePass->QuerryAttribute("texMulti"))
  {
    const glm::mat4 iden = glm::identity<glm::mat4>();
    _activePass->WriteAttribute("texMulti", (void *)&iden[0][0]);
  }
  if (enableLighting) {
    glm::vec3 pos = mainCamera.Position();
    _activePass->WriteAttribute("eye_position", &pos);

  }
}

void Renderer::SetBindings(GLuint b, GLuint VAO)
{
  _activePass->SetBindings(b, VAO);
}

glm::vec2 Renderer::ToScreenSpace(glm::vec2 src)
{
  auto screenSize = glm::vec2(_window->w, _window->h);
  return (src + glm::vec2(mainCamera.Position()) * _zoom + glm::vec2((screenSize.x / 2.0f), -(screenSize.y / 2.0f)) * glm::vec2(1, -1));
}

void Renderer::RegisterCallBack(int stage, int id, renderCallBack fn)
{
  if (_activePass)
    _activePass->RegisterCallBack(static_cast<renderStage>(stage), id, fn);
}

void Renderer::DrawRect(glm::vec2 pos, glm::vec2 scale, float rot, uint depth)
{
  if (Stored())
    return;
  if (depth != UINT_MAX)
  {
    if (_window->primary == true)
    {
      _activePass->BindActiveFBO(depth);
      if (depth == 2)
      {
        _activePass->WriteAttribute("screenMatrix", &_projectionMatrix[0][0]);
      }
    }
    else
    {
      _activePass->BindActiveFBO(-1);
    }
  }
  if (_window->VAO == "")
  {
    _window->VAO = _activePass->MakeVAO(_window->name + "VAO");
  }
  if (_activePass->HasVAO(_window->VAO) == false)
  {
    _activePass->MakeVAO(_window->VAO);
  }
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return;
  SetMatrix({pos, -1750}, {scale, 0}, {rot, 0, 0});
  std::vector<Vertex> mesh = {
      {{-.5f, -.5f, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {0, 1}},
      {{.5f, -.5f, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {1, 1}},
      {{.5f, .5f, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {1, 0}},
      {{-.5f, .5f, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {0, 0}},
  };
  const ORB_Mesh m = ORB_Mesh(GL_TRIANGLE_FAN, mesh, glm::vec4(1, 1, 1, 1));

  glBindVertexArray(m.VAO());
  glBindBuffer(GL_ARRAY_BUFFER, m.Buffer());
  glDrawArrays(m.DrawMode(), 0, static_cast<int>(m.Size()));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  if (depth == 2)
    _activePass->WriteAttribute("screenMatrix", &_storedProjection[0][0]);
}

void Renderer::DrawMesh(ORB_Mesh const &v, uint depth)
{
  if (storedRender)
  {
    const_cast<ORB_Mesh &>(v).AddCall(_currentObject);
    return;
  }
  if (depth != UINT_MAX)
  {
    if (_window->primary == true)
    {
      _activePass->BindActiveFBO(depth);
      if (depth == 2)
      {
        _activePass->WriteAttribute("screenMatrix", &_projectionMatrix[0][0]);
      }
    }
    else
    {
      _activePass->BindActiveFBO(-1);
    }
  }

  if (_window->VAO == "")
  {
    _window->VAO = _activePass->MakeVAO(_window->name + "VAO");
  }
  if (_activePass->HasVAO(_window->VAO) == false)
  {
    _activePass->MakeVAO(_window->VAO);
  }
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return;

  glBindVertexArray(v.VAO());
  glBindBuffer(GL_ARRAY_BUFFER, v.Buffer());
  glDrawArrays(v.DrawMode(), 0, static_cast<int>(v.Size()));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  if (depth == 2)
    _activePass->WriteAttribute("screenMatrix", &_storedProjection[0][0]);
}

void Renderer::SetColor(glm::vec4 const &color)
{
  if (storedRender)
  {
    _currentObject.color = color;
    return;
  }

  if (_activePass->QuerryAttribute("globalColor") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4 component vector bound to name: globalColor" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR: Drawabled render stage must contain 4 component vector bound to name: globalColor");
  };
  _activePass->WriteAttribute("globalColor", (void *)&color);
}

void Renderer::SetMatrix(glm::vec3 const &pos, glm::vec3 const &scale)
{
  SetMatrix(pos, scale, glm::vec3(0));
}

void Renderer::SetMatrix(glm::vec3 const &pos, glm::vec3 const &scale, float rot)
{
  SetMatrix(pos, scale, glm::vec3(rot, 0, 0));
}

void Renderer::DrawIndexed(ORB_Mesh const &v, int count)
{
  if (storedRender)
  {
    const_cast<ORB_Mesh &>(v).AddCall(_currentObject);
    return;
  }

  if (_window->primary == true)
  {
    _activePass->BindActiveFBO(1);
  }
  else
  {
    _activePass->BindActiveFBO(-1);
  }

  if (_window->VAO == "")
  {
    _window->VAO = _activePass->MakeVAO(_window->name + "VAO");
  }
  if (_activePass->HasVAO(_window->VAO) == false)
  {
    _activePass->MakeVAO(_window->VAO);
  }
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return;
  glBindVertexArray(v.VAO());
  glBindBuffer(GL_ARRAY_BUFFER, v.Buffer());
  glDrawArraysInstanced(v.DrawMode(), 0, static_cast<int>(v.Size()), count);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::SetMatrix(glm::vec3 const &pos, glm::vec3 const &scale, glm::vec3 const &rot)
{
  glm::mat4 temp = glm::identity<glm::mat4>();
  glm::vec3 sca = scale;
  if (sca.z == 0)
    sca.z = 1;
  temp = glm::translate(temp, pos);
  temp = glm::rotate(temp, rot.x, glm::vec3(0, 0, 1));
  temp = glm::rotate(temp, rot.y, glm::vec3(1, 0, 0));
  temp = glm::rotate(temp, rot.z, glm::vec3(0, 1, 0));
  temp = glm::scale(temp, sca);
  glm::mat3 inv = glm::inverse(glm::mat3(temp));
  glm::mat4 norm = glm::mat4(glm::transpose(inv));
  if (storedRender)
  {
    _currentObject.matrix = temp;
    _currentObject.normalMatrix = norm;
    return;
  }

  if (_activePass->QuerryAttribute("objectMatrix") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR : Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix");
  };
  _activePass->WriteAttribute("objectMatrix", &temp);
  if (_activePass->QuerryAttribute("normalMatrix") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4x4 matrix bound to name: normalMatrix" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR : Drawabled render stage must contain 4x4 matrix bound to name: normalMatrix");
  };

  _activePass->WriteAttribute("normalMatrix", &norm);
}

void Renderer::EnableLighting(bool value)
{
  enableLighting = value;
}
int StoredUpdate()
{
  std::vector<ORB_Mesh *> const &meshes = MeshLibrary::Instance()->GetMeshes();
  std::string fbo = "Primary 1";
  local->BindActiveFBO(local->GetFBOByName(fbo));
  local->SetBufferBase("RenderBuffer", 0);
  local->SetBufferBase("MaterialBuffer",1);
  local->WriteBuffer("MaterialBuffer", sizeof(Renderer::MaterialInfo) * local->_materials.size(), local->_materials.data());
  local->WriteRenderConstantsHere();
  for (auto &mesh : meshes)
  {

  //Log(Message, mesh->DrawMode(), mesh->path);
    mesh->Render();
    mesh->Reset();
  }
  return 0;
}
void Renderer::EnableStoredRender(bool value)
{
  storedRender = value;
  if (custom == false)
  {

    delete _activePass;
    if (storedRender)
    {
      _activePass = new RenderPass(1);
      _activePass->RegisterCallBack(renderStage::PrimaryRender, 0, StoredUpdate);
    }
    else
    {
      _activePass = new RenderPass(0);
    }
  }
}

void Renderer::SetLight(glm::vec4 pos, glm::vec3 color)
{
  if (enableLighting)
  {
    if (QueryAndSet("default") || QueryAndSet("primary"))
    {
      _activePass->WriteAttribute("light_position", &pos);
      _activePass->WriteAttribute("light_color", &color);
    }
  }
}

int ShadowPass()
{



  return 0;
}

void Renderer::EnableShadows(bool b) 
{
  _enableShadows = b;
  if(b)
  {
    _activePass->RegisterCallBack(renderStage::PreRender, 0, ShadowPass);

  }

}
void Renderer::SetMaterial(glm::vec3 diff, glm::vec3 spec, float specExp)
{
  if (storedRender)
  {
    auto l = std::find_if(_materials.begin(), _materials.end(), [&diff, &spec, specExp](MaterialInfo const &material) -> bool
                          { return material.diff == diff && material.spec == spec && material.specExp == specExp; });
    if (l == _materials.end())
    {
      _currentObject.materialID = _materials.size();
      _materials.push_back({.diff = diff, .spec = spec, .specExp = specExp});
    }
    else
    {
      _currentObject.materialID = l - _materials.begin();
    }
    return;
  }

  if (enableLighting)
  {
    if (QueryAndSet("default") || QueryAndSet("primary"))
    {
      if (_activePass->QuerryAttribute("diffuse_coefficient") == true)
      {
        _activePass->WriteAttribute("diffuse_coefficient", &diff);
      }
      if (_activePass->QuerryAttribute("specular_coefficient") == true)
      {
        _activePass->WriteAttribute("specular_coefficient", &spec);
      }
      if (_activePass->QuerryAttribute("specular_exponent") == true)
      {
        _activePass->WriteAttribute("specular_exponent", &specExp);
      }
    }
  }
}

void Renderer::SetMaterial(int id)
{
  if (id < 0 || id >= _materials.size())
    return;
  _currentObject.materialID = id;
}

glm::vec2 Renderer::GetWindowSize(Window *w)
{
  int x, y;
  return SDL_GetWindowSize(w->window, &x, &y), _window->w = x, _window->h = y, glm::vec2(x, y);
}

void Renderer::SetMatrix(glm::mat4 const &matrix)
{

  if (storedRender)
  {
    glm::mat3 inv = glm::inverse(glm::mat3(matrix));
    glm::mat4 norm = glm::mat4(glm::transpose(inv));
    _currentObject.matrix = matrix;
    _currentObject.normalMatrix = norm;

    return;
  }

  if (_activePass->QuerryAttribute("objectMatrix") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR : Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix");
  };
  _activePass->WriteAttribute("objectMatrix", (void *)&matrix);
}

void Renderer::Update()
{
  //Log(Message, "Updated");

  while (_activePass->CurrentStage() != renderStage::PostFrameSwap)
  {
    _activePass->Update();
    _activePass->RunStage();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CheckError(__LINE__);
  _activePass->Update();
  _activePass->RunStage();

  SetActiveWindow(defaultWindow);
  _activePass->FlattenFBOs();

  if (activeWindows.size() > 1)
    glFlush();
  for (auto win : activeWindows)
  {
    SetActiveWindow(win);
    SDL_GL_SwapWindow(win->window);
    glClearColor(win->r, win->g, win->b, win->a);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  SetActiveWindow(defaultWindow);
  glClearColor(defaultWindow->r, defaultWindow->g, defaultWindow->b, defaultWindow->a);

  UpdateRenderConstants();
  CheckError(__LINE__);
  _activePass->ResetRender();

  // SDL_UpdateWindowSurface(_window);
  CheckError(__LINE__);

#if LOG_WINDOW_SWAPS
  std::cout << "===============" << std::endl;
#endif
}

void Renderer::SetProjectionMode(int i)
{
  _projection = i;
}

void Renderer::SetActiveTexture(ORB_Texture *t)
{
  if (_activePass->QuerryAttribute("textured") == false)
  {
    std::cerr << "ORB ERROR: To use textures,  render stage must contain int bound to name: textured" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR : To use textures, render stage must contain int bound to name: textured");
  };
  if (_activePass->QuerryAttribute("tex") == false)
  {
    std::cerr << "ORB ERROR: To use textures, render stage must contain Sampler bound to name: tex" << std::endl;
    throw std::invalid_argument(
        "ORB ERROR : To use textures, render stage must contain Sampler bound to name: tex");
  }
  if (t == nullptr)
  {
    _activePass->WriteAttribute("textured", (void *)&zero);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
  _activePass->WriteAttribute("tex", (void *)&zero);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, t->texture());

  _activePass->WriteAttribute("textured", (void *)&one);
}

void Renderer::BindBuffer(std::string buffer)
{
  _activePass->BindBuffer(buffer);
}

void Renderer::UnbindBuffer(std::string buffer)
{
  _activePass->UnBindBuffer(buffer);
}

bool Renderer::QueryAndSet(std::string name)
{
  if (_activePass->QuerryStage(name))
  {
    _activePass->SetSpecificStage(name);
    return true;
  }
  return false;
}
void Renderer::ResizeFBOs()
{
  _activePass->ResizeFBOs();
}

void Renderer::UpdateRenderConstants()
{

  enum projtype
  {
    ortho,
    perspective,
  };
  _windowSize = GetWindowSize(_window);
  if (QueryAndSet("default") == false &&
      QueryAndSet("primary") == false)
  {
    std::cerr << "ORB ERROR: Main rendering stage must be named either 'default' or 'primary'" << std::endl;
    throw std::invalid_argument("ORB ERROR: Main rendering stage must be names either 'default' or 'primary'");
  }

  if (_activePass->QuerryAttribute("screenMatrix") == false)
  {
    std::cerr << "ORB ERROR: Main rendering stage must have mat4 named 'screenMatrix'. This matrix will be used for camera and projection" << std::endl;
    throw std::invalid_argument("ORB ERROR : Main rendering stage must have mat4 names 'screenMatrix'.This matrix will be used for camera and projection");
  }

  if (_activePass->QuerryAttribute("zoom") == false)
  {
    std::cerr << "ORB ERROR: Main rendering stage must have float named 'zooom'. This float will be used for proper zooming of camera" << std::endl;
    throw std::invalid_argument("ORB ERROR: Main rendering stage must have float named 'zooom'. This float will be used for proper zooming of camera");
  }
  if (enableLighting)
  {
    if (_activePass->QuerryAttribute("enableLighting") == false)
    {
      std::cerr << "ORB ERROR: Main rendering stage must have bool/int named 'enableLighting'. This will be used to turn on and off lighting" << std::endl;
      throw std::invalid_argument("ORB ERROR: Main rendering stage must have bool/int named 'enableLighting'. This will be used to turn on and off lighting");
    }
    _activePass->WriteAttribute("enableLighting", const_cast<int *>(&one));
  }

  glm::mat4 camMat = mainCamera.GetMatrix();
  switch (_projection)
  {
  case ortho:
    _projectionMatrix = glm::ortho(
        -_windowSize.x / 2.0f,
        _windowSize.x / 2.0f,
        -_windowSize.y / 2.0f,
        _windowSize.y / 2.0f,
        0.f,
        10000000.f);
    _storedProjection = _projectionMatrix * camMat;
    _uiProjection = _projectionMatrix;
    break;
  case perspective:
    glm::vec3 camPos = camMat * glm::vec4(0, 0, 0, 1);
    glm::vec3 lookatPos = camMat * glm::vec4(0, 0, -1, 1);
    _projectionMatrix = glm::perspective(120.0f, _windowSize.x / _windowSize.y, 1.0f, 100000.0f) * glm::lookAt(camPos, lookatPos, glm::vec3(0, 1, 0));
    _storedProjection = _projectionMatrix * camMat;
    _uiProjection  = glm::ortho(
      -_windowSize.x / 2.0f,
      _windowSize.x / 2.0f,
      -_windowSize.y / 2.0f,
      _windowSize.y / 2.0f,
      0.f,
      10000000.f);
    break;
  }
  _activePass->WriteAttribute("screenMatrix", &_storedProjection[0][0]);
  _activePass->WriteAttribute("zoom", &_zoom);
  if (_activePass->QuerryAttribute("texMulti"))
  {
    const glm::mat4 iden = glm::identity<glm::mat4>();
    _activePass->WriteAttribute("texMulti", (void *)&iden[0][0]);
  }
  if (enableLighting) {
    glm::vec3 pos = mainCamera.Position();
    _activePass->WriteAttribute("eye_position", &pos);

  }
  glViewport(0, 0, _window->w, _window->h);
}

void Renderer::SetActiveFont(ORB_FontInfo const *f)
{
  _activeFont = const_cast<ORB_FontInfo *>(f);
}

ORB_FontInfo const *Renderer::ActiveFont()
{
  return _activeFont;
}

void Renderer::SetZoom(float f)
{
  _zoom = f;
  mainCamera.setZoom(f);
}

Camera &Renderer::GetCamera()
{
  return mainCamera;
}

void Renderer::SetDefualtRenderMode(int i)
{
  renderMode = i;
}

unsigned int _activePolyMode = GL_FILL;
void Renderer::SetFillMode(int i)
{
  _activePolyMode = GL_POINT + i;
  glPolygonMode(GL_FRONT_AND_BACK, _activePolyMode);
}

void Renderer::SetBlendMode(int z)
{
  glEnable(GL_BLEND);
  switch (z)
  {
  case 0:
    glDisable(GL_BLEND);
    break;
  case 1:
    glBlendEquation(GL_FUNC_ADD);
    break;
  case 2:
    glBlendEquation(GL_FUNC_SUBTRACT);
    break;
  case 3:
    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    break;
  case 4:
    glBlendEquation(GL_MIN);
    break;
  case 5:
    glBlendEquation(GL_MAX);
    break;
  }
}

void Renderer::BindTextureToUnit(ORB_Texture *tex, int texture)
{
  if (texture < 0 or texture > 31)
    throw std::runtime_error("Attempted to bind to non-existant texture Unit");
  glActiveTexture(GL_TEXTURE0 + texture);
  glBindTexture(GL_TEXTURE_2D, tex->texture());
}

void Renderer::BindTextureToUnit(uint tex, int unit)
{
  if (unit < 0 or unit > 31)
    throw std::runtime_error("Attempted to bind to non-existant texture Unit");
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, tex);
}

fboinfo Renderer::GetFBOByName(std::string &s)
{
  shaderBuffer res;

  if (s.contains("Primary"))
  {
    s.erase(s.begin(), s.begin() + s.find('y') + 1);
    int id = std::stoi(s);
    res = _activePass->GetFrameBuffer(id);
  }
  else if (s.contains("Secondary"))
  {
    s.erase(s.begin(), s.begin() + s.find('y') + 1);
    int id = std::stoi(s) + 3;
    res = _activePass->GetFrameBuffer(id);
  }
  else
  {
    res = _activePass->GetFrameBuffer(s);
  }
  return {res.first, res.second};
}

void Renderer::BindActiveFBO(fboinfo f)
{
  glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);
}

void Renderer::ClearFBO(fboinfo f)
{
  glClearColor(0, 0, 0, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return;
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ORB_Texture *Renderer::RenderText(const char *text, glm::vec4 const &color, int size)
{

  TTF_SetFontSize(_activeFont->font, size);
  ORB_Texture *t = TextureManager::Instance()->CreateFromMemeory(text, 0, 0, 0, 0);
  SDL_Rect rect = {0, 0};
  TTF_SizeText(_activeFont->font, text, &rect.w, &rect.h);
  if (t == nullptr)
  {
    SDL_Surface *tex = TTF_RenderText_Solid(
        _activeFont->font,
        text,
        SDL_Color(
            (char)(color.r * 255.f),
            (char)(color.g * 255.f),
            (char)(color.b * 255.f),
            (char)(color.a * 255.f)));
    SDL_PixelFormat *target = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    SDL_Surface *surface = SDL_ConvertSurface(tex, target, 0);
    SDL_FreeSurface(tex);
    SDL_FreeFormat(target);
    t = TextureManager::Instance()->CreateFromMemeory(text, surface->w, surface->h, 4, surface->pixels);
    Log(TraceLevels::High, "Created Text Mesh: ", text);
  }

  return t;
}
