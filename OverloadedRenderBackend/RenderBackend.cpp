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
#define LOG_WINDOW_SWAPS 0
 // Used for sending ponter to value containing true or false
const int zero = 0;
const int one = 1;
extern Window* defaultWindow;

void GLAPIENTRY
MessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam)
{
  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    type, severity, message);
}
extern std::vector<Window*> activeWindows;

void CheckError(int l)
{
#if _DEBUG
  int i = glGetError();
  if (i != 0)
  {
    std::cerr << "Line " << l << " : " << i << std::endl;
    throw i;
  }
#endif
}


Renderer::Renderer()
  : _window(nullptr)
{
  SDL_Init(SDL_INIT_VIDEO);

  const char* title = "Primary Window";
  int width = 1280, height = 720;

  _windowSize = { width, height };

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

  SDL_GL_MakeCurrent(_window->window, context);
  SDL_GL_SetSwapInterval(1);

  //glClearColor(204.f / 255.f, 154.f / 255.f, 254.f / 255.f, .25f);

  _window->color = { 0,0,0,0 };

  glClearColor(_window->r, _window->g, _window->b, _window->a);
  glClear(GL_COLOR_BUFFER_BIT);

  SDL_GL_SwapWindow(_window->window);

  glClear(GL_COLOR_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

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

Renderer::Renderer(const char* path) : Renderer()
{
  LoadRenderPass(path);
}

Window* Renderer::GetWindow()
{
  return _window;
}

Window* Renderer::MakeWindow(std::string s)
{
  Window* result = new Window();

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

  //glClearColor(204.f / 255.f, 154.f / 255.f, 254.f / 255.f, .25f);
  result->color = { 0.1f,0,0,0.2f };

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

void Renderer::SetActiveWindow(Window* w)
{
  glFlush();
#if LOG_WINDOW_SWAPS
  static int change = 0;
  std::cout << "Changing to window " << w << " Change #" << change++ << std::endl;
#endif
  if (w->context == nullptr || w->window == nullptr)
  {
    if (_window->context == nullptr || w->window == nullptr)
    {
      auto next = std::find_if(activeWindows.begin(), activeWindows.end(), [](Window* w) {return w->context != nullptr && w->window != nullptr; });
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
        delete this;
      }
    }
    return;
  }
  _window = w;
  auto fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  GLenum result;
  while (true)
  {
    result = glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)); //5 Second timeout
    if (result != GL_TIMEOUT_EXPIRED) break; //we ignore timeouts and wait until all OpenGL commands are processed!
  }
  if (SDL_GL_MakeCurrent(_window->window, _window->context) != 0)
    throw "Bad";
  assert(_window->window == SDL_GL_GetCurrentWindow() && "Window swap successful");
  UpdateRenderConstants();
}
// TODO: Figure out correcting secondary window rendering
void Renderer::DestroyWindow(Window* w)
{
  //if (w == defaultWindow)
    //running = false;
  SDL_DestroyWindow(w->window);
  auto res = std::find(activeWindows.begin(), activeWindows.end(), w);
  activeWindows.erase(res);
  w->window = nullptr;
  w->context = nullptr;
}

void Renderer::LoadRenderPass(std::string path)
{
  LoadRenderPass(path.c_str());
}

void Renderer::LoadRenderPass(const char* path)
{

  // TODO: Make this check for API version 

  if (_activePass != nullptr)
    delete _activePass;

  _activePass = new RenderPass(path);
  for (auto& w : activeWindows)
  {
    w->VAO = "";
  }
}

void Renderer::WriteBuffer(std::string buffer, size_t dataSize, void* data)
{
  _activePass->BindBuffer(buffer);
  _activePass->WriteBuffer(buffer, dataSize, data);
  _activePass->UnBindBuffer(buffer);
}

void Renderer::WriteUniform(std::string uniform, void* data)
{
  _activePass->WriteAttribute(uniform, data);
}

glm::vec2 Renderer::ToWorldSpace(glm::vec2 src)
{
  return src * glm::vec2(1, -1) + glm::vec2(-(_window->w / 2.0f), (_window->h / 2.0f));
}

void Renderer::DispatchCompute(int x, int y, int z)
{
  _activePass->DispatchCompute(x, y, z);
}

glm::vec2 Renderer::ToScreenSpace(glm::vec2 src)
{
  return (src + glm::vec2((_window->w / 2.0f), -(_window->h / 2.0f))) * glm::vec2(1, -1);
}

void Renderer::RegisterCallBack(int stage, int id, renderCallBack fn)
{
  if (_activePass)
    _activePass->RegisterCallBack(static_cast<renderStage>(stage), id, fn);
}

void Renderer::DrawRect(glm::vec2 pos, glm::vec2 scale, float rot, uint depth)
{
  if (_window->primary == true)
    _activePass->BindActiveFBO(depth);
  else
  {
    _activePass->BindActiveFBO(-1);
  }
  if (_window->VAO == "")
  {
    _window->VAO = std::move(_activePass->MakeVAO(_window->name + "VAO"));
  }
  _activePass->BindBuffer(_window->VAO);
  _activePass->BindBuffer("VBO");
  SetMatrix(glm::vec3(pos, 0), glm::vec3(scale, 1), rot);
  const Vertex mesh[4] = {
    {{-.5f, -.5f, 0, 1}, {1, 1, 1, 1}, {0,1}},
    {{-.5f,  .5f, 0, 1}, {1, 1, 1, 1}, {0,0}},
    {{ .5f,  .5f, 0, 1}, {1, 1, 1, 1}, {1,0}},
    {{ .5f, -.5f, 0, 1}, {1, 1, 1, 1}, {1,1}} };
  _activePass->WriteBuffer("VBO", 4 * sizeof(Vertex), (void*)mesh);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  _activePass->UnBindBuffer("VBO");
  _activePass->UnBindBuffer(_window->VAO);
}

void Renderer::DrawMesh(std::vector<Vertex> const& v, uint depth, int poly)
{
  if (_window->primary == true)
    _activePass->BindActiveFBO(depth);
  else
  {
    _activePass->BindActiveFBO(-1);
  }

  if (_window->VAO == "")
  {
    _window->VAO = std::move(_activePass->MakeVAO(_window->name + "VAO"));
  }
  _activePass->BindBuffer(_window->VAO);
  _activePass->BindBuffer("VBO");
  _activePass->WriteBuffer("VBO", v.size() * sizeof(Vertex), (void*)v.data());
  glDrawArrays(poly, 0, static_cast<int>(v.size()));
  _activePass->UnBindBuffer("VBO");
  _activePass->UnBindBuffer(_window->VAO);
  CheckError(__LINE__);
}

void Renderer::SetColor(glm::vec4 color)
{
  if (_activePass->QuerryAttribute("globalColor") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4 component vector bound to name: globalColor" << std::endl;
    throw std::invalid_argument(
      "ORB ERROR: Drawabled render stage must contain 4 component vector bound to name: globalColor");
  };
  _activePass->WriteAttribute("globalColor", &color);
}

void Renderer::SetMatrix(glm::vec3 pos, glm::vec3 scale)
{
  SetMatrix(pos, scale, glm::vec3(0));
}

void Renderer::SetMatrix(glm::vec3 pos, glm::vec3 scale, float rot)
{
  SetMatrix(pos, scale, glm::vec3(rot, 0, 0));
}

void Renderer::SetMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot)
{
  constexpr glm::vec3 posChange = glm::vec3(1, 1, 1);
  glm::mat4 temp = glm::identity<glm::mat4>();
  temp = glm::translate(temp, pos * posChange);
  temp = glm::rotate(temp, rot.x, glm::vec3(0, 0, 1));
  temp = glm::rotate(temp, rot.y, glm::vec3(1, 0, 0));
  temp = glm::rotate(temp, rot.z, glm::vec3(0, 1, 0));
  temp = glm::scale(temp, scale);

  if (_activePass->QuerryAttribute("objectMatrix") == false)
  {
    std::cerr << "ORB ERROR: Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix" << std::endl;
    throw std::invalid_argument(
      "ORB ERROR : Drawabled render stage must contain 4x4 matrix bound to name: objectMatrix");
  };
  _activePass->WriteAttribute("objectMatrix", &temp);
}

glm::vec2 Renderer::GetWindowSize()
{
  int x, y;
  return  SDL_GetWindowSize(_window->window, &x, &y), _window->w = x, _window->h = y, glm::vec2(x, y);
}

void Renderer::Update()
{
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
  _activePass->ResetRender();

  //SDL_UpdateWindowSurface(_window);
  CheckError(__LINE__);

#if LOG_WINDOW_SWAPS
  std::cout << "===============" << std::endl;
#endif 
}

void Renderer::SetProjectionMode(int i)
{
  _projection = i;
}

void Renderer::SetActiveTexture(Texture* t)
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
    _activePass->WriteAttribute("textured", (void*)&zero);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
  }
  _activePass->WriteAttribute("tex", (void*)&zero);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, t->texture());

  _activePass->WriteAttribute("textured", (void*)&one);
}

void Renderer::UpdateRenderConstants()
{

  enum projtype
  {
    ortho,
    perspective,
  };
  _windowSize = GetWindowSize();
  if (_activePass->QuerryStage("default"))
  {
    _activePass->SetSpecificStage("default");

    glm::mat4 _projectionMatrix;
    glm::mat4x4 camMat = mainCamera.GetMatrix();
    switch (_projection)
    {
    case ortho:
      _projectionMatrix = glm::ortho(
        -_windowSize.x / 2.0f,
        _windowSize.x / 2.0f,
        -_windowSize.y / 2.0f,
        _windowSize.y / 2.0f,
        0.f,
        1000.f) * camMat;
      break;
    case perspective:
      glm::vec3 camPos = camMat * glm::vec4(0, 0, 0, 1);
      glm::vec3 lookatPos = camMat * glm::vec4(0, 0, 2, 1);
      _projectionMatrix = glm::perspective(120.0f, _windowSize.x / _windowSize.y, 1.0f, 1500.0f) * glm::lookAt(camPos, lookatPos, glm::vec3(0, 1, 0)) * camMat;
      break;
    }
    _activePass->WriteAttribute("screenMatrix", &_projectionMatrix[0][0]);

  }

}

void Renderer::SetActiveFont(FontInfo const* f)
{
  _activeFont = const_cast<FontInfo*>(f);
}


FontInfo const* Renderer::ActiveFont()
{
  return _activeFont;
}


Texture* Renderer::RenderText(const char* text, glm::vec4 const& color, int size)
{

  TTF_SetFontSize(_activeFont->font, size);
  Texture* t = TextureManager::Instance()->CreateFromMemeory(text, 0, 0, 0, 0);
  SDL_Rect rect = { 0, 0 };
  TTF_SizeText(_activeFont->font, text, &rect.w, &rect.h);
  if (t == nullptr)
  {
    SDL_Surface* tex = TTF_RenderText_Solid(
      _activeFont->font,
      text,
      SDL_Color(
        (char)(color.r * 255.f),
        (char)(color.g * 255.f),
        (char)(color.b * 255.f),
        (char)(color.a * 255.f)));
    GLenum mode = GL_RGBA;
    SDL_PixelFormat* target = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    SDL_Surface* surface = SDL_ConvertSurface(tex, target, 0);
    SDL_FreeSurface(tex);
    SDL_FreeFormat(target);
    SDL_PixelFormat* f = surface->format;
    t = TextureManager::Instance()->CreateFromMemeory(text, surface->w, surface->h, 4, surface->pixels);
    Log(TraceLevels::High, "Created Text Mesh: ", text);
  }

  return t;

}



