/*********************************************************************
 * @file   RenderPass.cpp
 * @brief  Implementation for the renderpass class
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   October 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#include "pch.h"
#include "RenderBackend.h"
#include "RenderPass.h"
#include "ShaderStage.h"
#include "Stream.h"
#include <algorithm>
#include <tuple>

#include "ShaderLog.hpp"
void CheckError(int);

extern std::vector<Window*> activeWindows;
extern Window* defaultWindow;
extern unsigned int _activePolyMode;
void RenderPass::WriteAttribute(std::string s, void* data)
{
  std::get<2>(_activeShaderStage)->WriteAttribute(s, data);
}

void RenderPass::WriteSubBufferData(std::string s, int index, size_t structSize, void* data)
{
  std::get<2>(_activeShaderStage)->WriteSubBufferData(s, index, structSize, data);
}

void RenderPass::SetBufferBase(std::string buffer, int base)
{
  std::get<2>(_activeShaderStage)->SetBufferBase(buffer, base);
}

void RenderPass::FlattenFBOs()
{
  const SDL_Window* const pr = SDL_GL_GetCurrentWindow();
  if (pr != defaultWindow->window)
    return;

  auto& s = _flattenStage;
  s->SetActive();
  //std::cout << "Flattening" << std::endl;

  auto& fbos = GetPrimaryFBOs();
  constexpr int bindings[1] = { 1 };
  s->WriteAttribute("FBO", (void*)&bindings[0]);
  struct Vertex
  {
    glm::vec2 pos;
    glm::vec2 tex;
  };
  // This essentially an a mesh in NDC
  const Vertex mesh[4] = {
  {{-1.f, -1.f}, {0, 0}},
  {{-1.f,  1.f}, {0, 1}},
  {{ 1.f,  1.f}, {1, 1}},
  {{ 1.f, -1.f}, {1, 0}} };
  BindActiveFBO(-1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  s->BindBuffer("VAO");
  s->BindBuffer("VBO");
  s->WriteBuffer("VBO", _countof(mesh) * sizeof(Vertex), (void*)mesh);
  // glBlendEquation(GL_MAX);
  glActiveTexture(GL_TEXTURE1);
  auto f = std::get<2>(fbos[0]);
  glBindTexture(GL_TEXTURE_2D, f);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  f = std::get<2>(fbos[1]);
  glBindTexture(GL_TEXTURE_2D, f);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  f = std::get<2>(fbos[2]);
  glBindTexture(GL_TEXTURE_2D, f);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  s->UnBindBuffer("VBO");
  s->UnBindBuffer("VAO");
  glPolygonMode(GL_FRONT_AND_BACK, _activePolyMode);

}

void RenderPass::RegisterCallBack(renderStage stage, int id, renderCallBack fn)
{
  _renderCallbacks.push_back({ stage, id, fn });
}

renderStage RenderPass::CurrentStage()
{
  return _activeStage;
}

std::pair<GLuint, GLuint> RenderPass::GetFrameBuffer(std::string s)
{
  auto& fbo = _additionalFBOs[s];
  if (std::get<1>(fbo) != 0)
    return { std::get<1>(fbo), std::get<2>(fbo) };
  else
    return { 0, 0 };
}

std::pair<GLuint, GLuint> RenderPass::GetFrameBuffer(int id)
{
  auto find = [this, id](std::pair<std::string, frameBufferObject> const& a) -> bool {
    return (std::get<0>(a.second) == _activeStage) && (std::get<1>(a.second) == id);
    };
  switch (_activeStage)
  {
  case renderStage::PrimaryRender:
    if (id < 3)
      return { std::get<1>(_primaryFBOs[id]), std::get<2>(_primaryFBOs[id]) };
    else
    {
      auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
      if (it != _additionalFBOs.end())
        return { std::get<1>(it->second), std::get<2>(it->second) };
      else
        return { 0, 0 };
    }
    break;
  case renderStage::SecondaryRender:
    if (id < 3)
      return { std::get<1>(_secondaryFBOs[id]), std::get<2>(_secondaryFBOs[id]) };
    else
    {
      auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
      if (it != _additionalFBOs.end())
        return { std::get<1>(it->second), std::get<2>(it->second) };

      else
        return { 0, 0 };
    }
    break;
  default:
    auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
    if (it != _additionalFBOs.end())
      return { std::get<1>(it->second), std::get<2>(it->second) };

    else
      return { 0, 0 };
  }
}

std::array<frameBufferObject, 3> const& RenderPass::GetPrimaryFBOs()
{
  return _primaryFBOs;
}

std::array<frameBufferObject, 3> const& RenderPass::GetSecondaryFBOs()
{
  return _secondaryFBOs;
}

void RenderPass::ResetRender()
{

  _activeStage = renderStage::PreRender;
  auto lambdaShaderPass = [this](std::pair<const std::string, ShaderPass> s) {
    return (std::get<0>(s.second) == _activeStage) && (std::get<1>(s.second) == 0);
    };
  auto it = std::find_if(_passess.begin(), _passess.end(), lambdaShaderPass);
  if (it != _passess.end())
  {
    _activeShaderStage = it->second;
    std::get<1>(_activeShaderStage) = -1;
  }
  glClearColor(0, 0, 0, 0);
  for (int i = 0; i < 3; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(_primaryFBOs[i]));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return;
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(_secondaryFBOs[i]));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return;
    glClear(GL_COLOR_BUFFER_BIT);
  }

  for (auto& fbo : _additionalFBOs)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(fbo.second));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return;
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

void RenderPass::ResizeFBOs()
{
  auto screenSize = glm::vec2(defaultWindow->w, defaultWindow->h);
  for (auto& p : _primaryFBOs)
  {
     glBindFramebuffer(GL_FRAMEBUFFER, std::get<2>(p));
    glBindTexture(GL_TEXTURE_2D, std::get<2>(p));
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      static_cast<GLsizei>(screenSize.x),
      static_cast<GLsizei>(screenSize.y),
      0,
      GL_RGBA,
      GL_FLOAT,
      NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == 0 && "But I don't know what");
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  for (auto& p : _secondaryFBOs)
  {
    // glBindFramebuffer(GL_FRAMEBUFFER, std::get<2>(p));
    glBindTexture(GL_TEXTURE_2D, std::get<2>(p));
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      static_cast<GLsizei>(screenSize.x),
      static_cast<GLsizei>(screenSize.y),
      0,
      GL_RGBA,
      GL_FLOAT,
      NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(
      glGetError() == 0 &&
      "If that means what I think it means we're in trouble, big trouble");
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  for (auto& adi : _additionalFBOs)
  {
    auto& fbo = adi.second;
    glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(fbo));
    glDeleteTextures(1, &std::get<2>(fbo));
    glGenTextures(1, &std::get<2>(fbo));
    glBindTexture(GL_TEXTURE_2D, std::get<2>(fbo));
    glTexStorage2D(
      GL_TEXTURE_2D,
      1,
      GL_RGBA32F,
      static_cast<int>(screenSize.x),
      static_cast<int>(screenSize.y));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, std::get<2>(fbo), 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(
      glGetError() == 0 && "And if he is as bananas as you say, I'm not taking any chances");
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  //glViewport(0, 0, defaultWindow->vx, defaultWindow->vh);
}

void RenderPass::ResizeSpecificFBO(std::string s, glm::vec2 const& newSize)
{
  auto& frameBuffer = _additionalFBOs[s];
  // --------------------------
  // Ahhhh, don't we all love c++'s antics. It is technically legal to store a reference inside a
  // std data structure HOWEVER it would be meaningless to because it would never be usable how
  // you want SOOOOO instead of making it work, they decided to add the std::reference_wrapper
  // class which does do what you want fukin hurray
  //    - Lorenzo
  // --------------------------
  std::pair<std::reference_wrapper<GLuint>, std::reference_wrapper<GLuint>> fbo = {
      std::get<1>(frameBuffer), std::get<2>(frameBuffer) };
  glBindFramebuffer(GL_FRAMEBUFFER, fbo.first.get());
  // To resize a texture with how we are making them
  // we must delete it first
  glDeleteTextures(1, &fbo.second.get());
  glGenTextures(1, &fbo.second.get());
  glBindTexture(GL_TEXTURE_2D, fbo.second.get());
  glTexStorage2D(
    GL_TEXTURE_2D, 1, GL_RGBA32F, static_cast<int>(newSize.x), static_cast<int>(newSize.y));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.second, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool RenderPass::QuerryAttribute(std::string s)
{
  return std::get<2>(_activeShaderStage)->QuerryAttribute(s);
}

bool RenderPass::QuerryStage(std::string s)
{
  return _passess.find(s) != _passess.end();
}

int RenderPass::MakeFBO(std::string s)
{
  GLuint fbo;
  GLuint texture;
  glGenFramebuffers(1, &fbo);
  CheckError(__LINE__);
  glGenTextures(1, &texture);
  auto screenSize = glm::vec2(1280, 720);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  CheckError(__LINE__);
  glBindTexture(GL_TEXTURE_2D, texture);
  CheckError(__LINE__);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    static_cast<GLsizei>(screenSize.x),
    static_cast<GLsizei>(screenSize.y),
    0,
    GL_RGBA,
    GL_FLOAT,
    NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  frameBufferObject newest = { renderStage::PrimaryRender, fbo, texture };
  _additionalFBOs[s] = newest;
  return fbo;
}

std::string RenderPass::MakeVAO(std::string s)
{
  return std::get<2>(_activeShaderStage)->MakeExtraVAO(s);
}

void RenderPass::DispatchCompute(int x, int y, int z)
{
  std::get<2>(_activeShaderStage)->Dispatch(x, y, z);
}


void RenderPass::SetupDefaultFBOs()
{
  GLuint defaultFBOs[6] = { 0 };
  GLuint Textures[6] = { 0 };
  glGenFramebuffers(6, defaultFBOs);
  CheckError(__LINE__);
  glGenTextures(6, Textures);
  auto screenSize = glm::vec2(1280, 720);/* GLBackend::GetWindowDimensions();*/
  for (int i = 0; i < 6; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBOs[i]);
    CheckError(__LINE__);
    glBindTexture(GL_TEXTURE_2D, Textures[i]);
    CheckError(__LINE__);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      static_cast<GLsizei>(screenSize.x),
      static_cast<GLsizei>(screenSize.y),
      0,
      GL_RGBA,
      GL_FLOAT,
      NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Textures[i], 0);
    CheckError(__LINE__);
  }
  CheckError(__LINE__);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _primaryFBOs[0] = frameBufferObject(renderStage::PrimaryRender, defaultFBOs[0], Textures[0]);
  _primaryFBOs[1] = frameBufferObject(renderStage::PrimaryRender, defaultFBOs[1], Textures[1]);
  _primaryFBOs[2] = frameBufferObject(renderStage::PrimaryRender, defaultFBOs[2], Textures[2]);
  _secondaryFBOs[0] =
    frameBufferObject(renderStage::SecondaryRender, defaultFBOs[3], Textures[3]);
  _secondaryFBOs[1] =
    frameBufferObject(renderStage::SecondaryRender, defaultFBOs[4], Textures[4]);
  _secondaryFBOs[2] =
    frameBufferObject(renderStage::SecondaryRender, defaultFBOs[5], Textures[5]);

}

RenderPass::RenderPass()
{
  SetupDefaultFBOs();
  ShaderStage* defaultRender = new ShaderStage(0);
  ShaderStage* flattenRender = new ShaderStage(1);

  _passess["default"] = { renderStage::PrimaryRender, 0, defaultRender };
  _flattenStage = flattenRender;
}

RenderPass::RenderPass(const char* f) : RenderPass(std::string(f))
{

}



RenderPass::RenderPass(std::string path) : _flattenStage(new ShaderStage(1))
{

  Stream file(path);
  if (file.Open() == false)
    throw std::invalid_argument("Bad file path");
  // Read each line and check for <
  std::string token;
  auto screenSize = glm::vec2(1280, 720);/* GLBackend::GetWindowDimensions();*/
  SetupDefaultFBOs();
  while (file.isEOF() != true)
  {
    // if we fine a < then set what section we are reading
    token = makeLowerCase(file.readString());
    if (token.find('<') != std::string::npos)
    {
      if (token == "<stages>")
      {
        while (true)
        {
          token = makeLowerCase(file.readString());
          if (token == "</stages>")
            break;
          else
          {
            // Find the equal sign
            const size_t brq = token.find('[');
            // save the name
            std::string name = token.substr(0, brq);
            // erase up to and afterthe equal sign
            token = token.erase(0, brq + 1);
            renderStage stage = static_cast<renderStage>(std::stoi(token));
            const size_t eq = token.find('=');
            token = token.erase(0, eq + 1);
            unsigned int id = std::stoi(token);
            ShaderStage* s = new ShaderStage(name + ".meta");
            s->parent = this;
            _passess[name.substr(name.rfind('/') + 1)] = {stage, id, s};
          }
        }
      }
      else if (token == "<fbos>")
      {
        while (true)
        {
          token = makeLowerCase(file.readString());
          if (token == "</fbos>")
            break;
          else
          {
            // Find the equal sign
            const size_t eq = token.find('=');
            // save the name
            std::string name = token.substr(0, eq);
            // erase up to and afterthe equal sign
            token = token.erase(0, eq + 1);
            renderStage stage = static_cast<renderStage>(std::stoi(token));
            GLuint newFBO;
            GLuint newTexture;
            glGenFramebuffers(1, &newFBO);
            glGenTextures(1, &newTexture);
            glBindFramebuffer(GL_FRAMEBUFFER, newFBO);
            glBindTexture(GL_TEXTURE_2D, newTexture);
            glTexStorage2D(
              GL_TEXTURE_2D,
              1,
              GL_RGBA32F,
              static_cast<int>(screenSize.x),
              static_cast<int>(screenSize.y));

            glFramebufferTexture2D(
              GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTexture, 0);
            _additionalFBOs[name] = { stage, newFBO, newTexture };
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
          }
        }
      }
    }
  }
  
}

RenderPass::RenderPass(RenderPass const& r)
{
}

RenderPass& RenderPass::operator=(RenderPass const&)
{
  // TODO: insert return statement here
  return *this;
}

RenderPass::~RenderPass()
{
  for (auto& pass : _passess) 
  {
    delete std::get<2>(pass.second);
  }
  for (auto& fbo : _additionalFBOs) 
  {
    glDeleteFramebuffers(1,  & std::get<1>(fbo.second));
    glDeleteTextures(1, &std::get<2>(fbo.second));

  }
  for (auto& fbo : _primaryFBOs)
  {
    glDeleteFramebuffers(1, &std::get<1>(fbo));
    glDeleteTextures(1, &std::get<2>(fbo));

  }
  for (auto& fbo : _secondaryFBOs)
  {
    glDeleteFramebuffers(1, &std::get<1>(fbo));
    glDeleteTextures(1, &std::get<2>(fbo));

  }
}


void RenderPass::Update()
{

  // Get current ShaderStage ID
  // Find all callbacks with same render stage and id
  // Call all callbacks
  // After all callbacks get called move to next stage
  signed int id = static_cast<int>(std::get<1>(_activeShaderStage));
  // Continuously search till we find either a valid render stage or don't
  bool broken = false;
  while (!broken)
  {
    auto lambdaShaderPass = [this, id](std::pair<const std::string, ShaderPass> s) {
      return (std::get<0>(s.second) == _activeStage) && (std::get<1>(s.second) == (id + 1));
      };
    auto st = std::find_if(_passess.begin(), _passess.end(), lambdaShaderPass);
    if (st != _passess.end())
    {
      _activeShaderStage = st->second;
      broken = true;
    }
    else
    {
      // Increase the stage we are on
      _activeStage = static_cast<renderStage>((static_cast<int>(_activeStage) + 1));
      // Check if we are on the end stage
      if (_activeStage == renderStage::End)
      {
        // Back up 1 stage and return
        _activeStage = static_cast<renderStage>((static_cast<int>(_activeStage) - 1));
        return;
      }
      id = -1;
    }
  }
}

void RenderPass::RunStage()
{
  renderCallBack lastfn = nullptr;

  // ------------------------------------------------------------
  // Note:
  // ------------------------------------------------------------
  // This is a lambda function, they are really cool and are essentially a function within a
  // function that acts like a variable.
  // that can be passed as is to another function or used as a template param
  // The first brackets part is called the capture list, bassically the things inside there
  // are variables inside the lambda that aren't going to be messed with Then you have the
  // parameters then the body The arrow defines what the return is, just like rust however if
  // you have a return statement in a lambda then the compiler can figure out what the return
  // is automatically
  //     - Lorenzo
  // ------------------------------------------------------------
  auto lambdaFunctions = [this, lastfn](callback const& s) -> bool {
    return (std::get<0>(s) == _activeStage) &&
      (std::get<1>(s) == std::get<1>(_activeShaderStage)) && (std::get<2>(s) != lastfn);
    };
  auto it = std::find_if(_renderCallbacks.begin(), _renderCallbacks.end(), lambdaFunctions);
  auto& shader = std::get<2>(_activeShaderStage);
  shader->SetActive();
  auto lastIterator = _renderCallbacks.end();
  while (it != _renderCallbacks.end() && it != lastIterator)
  {
    //Log(TraceLevels::CONSOLESPAMMMM, "Calling callback\n");
    lastfn = std::get<2>(*it);
    int err = lastfn();
    if (err != 0)
    {
      Log(Error, "Shader function exited early due to error:", err);
    }
    loopIt:
    auto lambdaFunctionssss = [this, lastfn](callback const& s) -> bool {
      return (std::get<0>(s) == _activeStage) &&
        (std::get<1>(s) == std::get<1>(_activeShaderStage)) &&
        (std::get<2>(s) != lastfn);
      };
    auto lit = std::find_if(it, _renderCallbacks.end(), lambdaFunctionssss);
    lastIterator = it;
    if (lit > lastIterator)
    {
      it = lit;
    }
    else if (lit < lastIterator) 
    {
      goto loopIt;
    }
  }
}

void RenderPass::SetSpecificStage(std::string s)
{

  auto& shader = std::get<2>(_passess[s]);
  _activeShaderStage = _passess[s];
  shader->SetActive();
}

void RenderPass::BindBuffer(std::string s)
{
  auto& shader = std::get<2>(_activeShaderStage);
  shader->BindBuffer(s);
}

void RenderPass::UnBindBuffer(std::string s)
{
  auto& shader = std::get<2>(_activeShaderStage);
  shader->UnBindBuffer(s);
}

void RenderPass::BindActiveFBO(int id)
{
  if (id == -1)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return;
  }
  auto find = [&](std::pair<std::string, frameBufferObject> const& a) -> bool {
    return (std::get<0>(a.second) == _activeStage) && (std::get<1>(a.second) == id);
    };
  switch (_activeStage)
  {
  case renderStage::PrimaryRender:
    if (id < 3)
      glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(_primaryFBOs[id]));
    else
    {
      auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
      if (it != _additionalFBOs.end())
        glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(it->second));
      else
      {
        Log(Error, "Attempted to bind non existant FBO");
        throw std::invalid_argument("Attempted to bind non existant FBO");
      }
    }
    break;
  case renderStage::SecondaryRender:
    if (id < 3)
      glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(_secondaryFBOs[id]));
    else
    {
      auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
      if (it != _additionalFBOs.end())
        glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(it->second));
      else
      {
        Log(Error, "Attempted to bind non existant FBO");

        throw std::invalid_argument("Attempted to bind non existant FBO");
      }
    }
    break;
  default:
    auto it = std::find_if(_additionalFBOs.begin(), _additionalFBOs.end(), find);
    if (it != _additionalFBOs.end())
      glBindFramebuffer(GL_FRAMEBUFFER, std::get<1>(it->second));
    else
    {
      Log(Error, "Attempted to bind non existant FBO");
      throw std::invalid_argument("Attempted to bind non existant FBO");
    }
    break;
  }
}

void RenderPass::UnBindActiveFBO()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass::WriteBuffer(std::string s, size_t dataSize, void* data)
{
  std::get<2>(_activeShaderStage)->WriteBuffer(s, dataSize, data);
}
