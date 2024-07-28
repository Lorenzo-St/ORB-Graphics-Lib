#include "pch.h"

#define ORB_EXPOSE_GLM
#include "OverloadedRenderBackend.h"
#include "RenderBackend.h"
#include "Textures.h"
#include "Mesh.h"
#include "TexturedMesh.h"
#include "Mesh Library.h"
#include "Fonts.h"

enum class Errors : int
{
  NoError = 0,
  ShutdownFailed = -50,
  InitFailure = -100,
  FontLoadFailure = 300,

};
void CheckError(int);
Renderer *active = nullptr;
Errors errorState = Errors::NoError;

ORB_Mesh *_activeMesh = nullptr;

std::vector<Window *> activeWindows;
Window *defaultWindow = nullptr;

KeyCallback keyFunction;
std::unordered_map<char, KEY_STATE> keys;

MouseButtonCallback mbuttonFunction;
std::unordered_map<MOUSEBUTTON, KEY_STATE> mouseButtons;

MouseMovmentCallback mMovementFunction;

WindowCallback windowFunction;

GenericCallback genericFunction;

void LogError(const char *s)
{
  std::cerr << "ORB ERROR: " << s << std::endl;
}

glm::vec3 Convert(Vector3D v)
{
  return glm::vec3(v.x, v.y, v.z);
}

glm::vec4 Convert(Vector4D v)
{
  return glm::vec4(v.r, v.g, v.b, v.a);
}

glm::vec2 Convert(Vector2D v)
{
  return glm::vec2(v.x, v.y);
}
Vector2D Convert(glm::vec2 v)
{
  return {v.x, v.y};
}

namespace orb
{
  ORB_SPEC void ORB_API Initialize()
  {
    if (active == nullptr)
      active = new Renderer();
    if (active == nullptr)
    {
      LogError("Failed to Initialize");
      errorState = Errors::InitFailure;
    }
    activeWindows.push_back(active->GetWindow());
    defaultWindow = active->GetWindow();
    ORB_Mesh::_backend = active;
  }

  ORB_SPEC void ORB_API RequestAPI(API_VERSION version)
  {
  }

  ORB_SPEC void ORB_API EnableDebugOutput(bool b)
  {
    active->EnableDebugOutput(b);
  }

  void CallKeyCallBack(char key, KEY_STATE newstate)
  {
    if (keyFunction != nullptr)
    {
      KEY_STATE &state = keys[key];
      if (newstate == KEY_STATE::RELEASED)
        state = KEY_STATE::RELEASED;
      else if (newstate == state)
      {
        state = KEY_STATE::HELD;
      }
      else if (newstate > state)
        state = newstate;

      keyFunction(key, state);
    }
  }

  bool IsFKey(SDL_KeyboardEvent *k)
  {
    return k->keysym.scancode > 57 && k->keysym.scancode < 70;
  }

  ORB_SPEC void ORB_API Update()
  {
    CheckError(__LINE__);
    active->Update();
    SDL_Event ev = {};
    while (SDL_PollEvent(&ev))
    {
      if (genericFunction != nullptr)
        genericFunction(&ev);
      switch (ev.type)
      {
      case SDL_WINDOWEVENT:
      {
        auto w = SDL_GetWindowFromID(ev.window.windowID);
        auto fin = [w](Window *wi)
        { return wi->window == w; };
        auto winPointer = std::find_if(activeWindows.begin(), activeWindows.end(), fin);

        switch (ev.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
        {

          if (winPointer != activeWindows.end())
          {
            active->DestroyWindow(*winPointer);
          }
        }
        break;
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_MINIMIZED:
          glm::vec2 oldSize = {(*winPointer)->w, (*winPointer)->h};
          bool foc = SDL_GetWindowGrab(w);
          SDL_GetWindowPosition(w, &((*winPointer)->x), &((*winPointer)->y));
          SDL_GetWindowSize(w, &((*winPointer)->w), &((*winPointer)->h));
          glm::vec2 newSize = {(*winPointer)->w, (*winPointer)->h};
          glm::vec2 ratio = ((newSize.y != 0) ? newSize
                                              : ((*winPointer)->w = static_cast<int>(oldSize.x),
                                                 (*winPointer)->h = static_cast<int>(oldSize.y),
                                                 oldSize)) /
                            oldSize;
          active->_zoom *= ratio.y;

          if (windowFunction != nullptr)
          {

            windowFunction(*winPointer, (*winPointer)->x, (*winPointer)->y, (*winPointer)->w, (*winPointer)->h, foc);
          }
          active->GetCamera().setZoom(active->_zoom);

          active->ResizeFBOs();
          break;
        }
      }
      break;
      case SDL_APP_TERMINATING:
        /* FALL THROUGH */
      case SDL_QUIT:
        active->running = false;
        break;
      case SDL_KEYDOWN:
        /* FALL THROUGH */
      case SDL_KEYUP:
        /* FALL THROUGH */
      case SDL_TEXTEDITING:
        /* FALL THROUGH */
      case SDL_TEXTINPUT:
        // std::cout << ev.key.keysym.sym << " " << +ev.key.state << std::endl;
        if (ev.key.state > 1)
          ev.key.state = 1;
        if (IsFKey(reinterpret_cast<SDL_KeyboardEvent *>(&ev)))
          ev.key.keysym.sym += 100;
        CallKeyCallBack(ev.key.keysym.sym, static_cast<KEY_STATE>(ev.key.state));
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        if (mbuttonFunction != nullptr)
        {
          MOUSEBUTTON b = static_cast<MOUSEBUTTON>(ev.button.button);
          KEY_STATE newstate = static_cast<KEY_STATE>(ev.button.state);

          KEY_STATE &state = mouseButtons[b];
          if (newstate == KEY_STATE::RELEASED)
            state = KEY_STATE::RELEASED;
          else if (newstate == state)
          {
            state = KEY_STATE::HELD;
          }
          else if (newstate > state)
            state = newstate;

          mbuttonFunction(b, state);
        }
        break;
      case SDL_MOUSEMOTION:
        if (mMovementFunction != nullptr)
        {
          SDL_MouseMotionEvent mme = ev.motion;
          mMovementFunction(mme.x, mme.y, mme.xrel, mme.yrel);
        }
        break;
      }
    }
  }

  ORB_SPEC void EnableLighting(bool value)
  {
    active->EnableLighting(value);
  }

  ORB_SPEC void EnableShadows(bool b)
  {
    active->EnableShadows(b);
  }

  ORB_SPEC void SetMaterialProperties(Vector3D diffuse, Vector3D specular, float specular_exponent)
  {
    active->SetMaterial(diffuse, specular, specular_exponent);
  }

  ORB_SPEC void SetMaterial(int id)
  {
    active->SetMaterial(id);
  }

  ORB_SPEC void SetLight(Vector4D pos, Vector3D color)
  {
    active->SetLight(pos, color);
  }

  ORB_SPEC void EnableStoredRender(bool b)
  {
    active->EnableStoredRender(b);
  }

  ORB_SPEC Window *CreateNewWindow()
  {
    Window *w = active->MakeWindow();
    activeWindows.push_back(w);
    return w;
  }

  ORB_SPEC Window *CreateNewWindow(std::string &name)
  {
    Window *w = active->MakeWindow(name);
    activeWindows.push_back(w);
    return w;
  }

  ORB_SPEC void SetActiveWindow(Window *w)
  {
    active->SetActiveWindow(w);
  }

  ORB_SPEC Window *RetrieveWindow(int index)
  {
    if (index < 0 || index >= activeWindows.size())
      return nullptr;
    return activeWindows[index];
  }

  ORB_SPEC void ORB_API SetWindowClearColor(Window *w, Vector4D color)
  {
    w->color = {color.r, color.g, color.b, color.a};
  }

  ORB_SPEC void ORB_API RegisterRenderCallback(int (*Callback)(), RENDER_STAGE stage, int index)
  {
    active->RegisterCallBack(static_cast<int>(stage), index, Callback);
  }

  ORB_SPEC void ORB_API RegisterKeyboardCallback(KeyCallback callback)
  {
    keyFunction = callback;
  }

  ORB_SPEC void ORB_API RegisterMouseButtonCallback(MouseButtonCallback callback)
  {
    mbuttonFunction = callback;
  }

  ORB_SPEC void ORB_API RegisterMouseMovementCallback(MouseMovmentCallback callback)
  {
    mMovementFunction = callback;
  }

  ORB_SPEC void ORB_API RegisterWindowCallback(WindowCallback callback)
  {
    windowFunction = callback;
  }

  ORB_SPEC void ORB_API RegisterMessageCallback(GenericCallback callback)
  {
    genericFunction = callback;
  }

  ORB_SPEC void ORB_API ShutDown()
  {
    try
    {
      for (auto &w : activeWindows)
      {
        active->DestroyWindow(w);
        delete w;
        w = nullptr;
      }
      activeWindows.clear();
      delete active;
      active = nullptr;
    }
    catch (...)
    {
      LogError("Shutdown Failure");
      errorState = Errors::ShutdownFailed;
    }
  }
  ORB_SPEC int ORB_API GetError()
  {
    return static_cast<int>(errorState);
  }
  ORB_SPEC void ORB_API DrawRect(float x, float y, float width, float height, int layer)
  {
    active->DrawRect({x, y}, {width, height}, 0, layer);
    SetUV(glm::identity<glm::mat4>());
  }
  ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale, int layer)
  {
    active->DrawRect({pos.x, pos.y}, {scale.x, scale.y}, 0, layer);
    SetUV(glm::identity<glm::mat4>());
  }
  ORB_SPEC void ORB_API SetWindowPosition(Window *w, int x, int y)
  {
    active->SetWindowPosition(w, x, y);
  }
  ORB_SPEC void ORB_API DrawRectAdvanced(float x, float y, float width, float height, float rotation, int layer)
  {
    active->DrawRect({x, y}, {width, height}, rotation, layer);
    SetUV(glm::identity<glm::mat4>());
  }
  ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation, int layer)
  {
    active->DrawRect({pos.x, pos.y}, {scale.x, scale.y}, rotation, layer);
    SetUV(glm::identity<glm::mat4>());
  }
  ORB_SPEC void ORB_API SetWindowScale(Window *w, int wi, int h)
  {
    active->SetWindowScale(w, wi, h);
  }
  ORB_SPEC void ORB_API SetWindowViewport(Window *w, int vx, int vy, int vw, int vh)
  {
    active->SetWindowViewPort(w, vx, vy, vw, vh);
  }
  ORB_SPEC void ORB_API SetWindowMax(Window *w)
  {
    active->SetWindowMaximized(w);
  }
  ORB_SPEC void ORB_API SetWindowFullScreen(Window *w, int type)
  {
    active->SetWindowFullScreen(w, type);
  }
  ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a)
  {
    active->SetColor({r / 255.f, g / 255.f, b / 255.f, a / 255.f});
  }
  ORB_SPEC bool ORB_API IsRunning()
  {

    return active != nullptr ? active->running : false;
  }
  ORB_SPEC Vector2D ORB_API ToScreenSpace(Vector2D v)
  {
    return Convert(active->ToScreenSpace(Convert(v)));
  }
  ORB_SPEC Vector2D ORB_API ToWorldSpace(Vector2D v)
  {
    return Convert(active->ToWorldSpace(Convert(v)));
  }
  ORB_SPEC void ORB_API SetProjectionMode(PROJECTION_TYPE p)
  {
    active->SetProjectionMode(static_cast<int>(p));
  }
  ORB_SPEC void ORB_API SetDefaultRenderMode(int i)
  {
    active->SetDefualtRenderMode(i);
  }
  ORB_SPEC void ORB_API DrawLine(Vector2D start, Vector2D end, int depth)
  {
    orb::DrawLine(Vector3D(start), Vector3D(end), depth);
  }
  ORB_SPEC void ORB_API DrawLine(Vector3D start, Vector3D end, int depth)
  {
    if (active->Stored())
      return;
    CheckError(__LINE__);
    int oldActive = _activePolyMode - GL_POINT;
    std::vector<Vertex> points = {
        {glm::vec4(static_cast<glm::vec3>(start), 1), {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0}},
        {glm::vec4(static_cast<glm::vec3>(end), 1), {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0}}};
    active->SetMatrix(glm::identity<glm::mat4>());
    active->SetFillMode(1);
    const ORB_Mesh m = ORB_Mesh(GL_LINES, points, glm::vec4(1, 1, 1, 1));
    active->DrawMesh(m, depth);
    active->SetFillMode(oldActive);
  }
  ORB_SPEC void ORB_API SetFillMode(int i)
  {
    active->SetFillMode(i);
  }
  ORB_SPEC std::vector<ORB_texture> const &ORB_API GetAllLoadedTextures()
  {
    return TextureManager::Instance()->GetTextures();
  }
  ORB_SPEC void ORB_API SetZoom(float z)
  {
    active->SetZoom(z);
  }
  ORB_SPEC Vector2D ORB_API GetWindowSize(Window *w)
  {
    return Convert(active->GetWindowSize(w));
  }
  ORB_SPEC Vector2D ORB_API GetCameraPosition()
  {
    return Convert(active->GetCamera().Position());
  }
  ORB_SPEC void ORB_API SetCameraPosition(Vector2D pos)
  {
    active->GetCamera().moveCamera(Convert(pos));
  }
  ORB_SPEC void ORB_API SetCameraRotation(Vector3D rot)
  {
    active->GetCamera().rotateCamera(Convert(rot));
  }
  ORB_SPEC float ORB_API GetZoom()
  {
    return active->GetCamera().GetZoom();
  }
  ORB_SPEC ORB_texture ORB_API LoadTexture(std::string &path)
  {
    return TextureManager::Instance()->LoadTexture(path);
  }
  ORB_SPEC ORB_texture ORB_API LoadTexture(const char *path)
  {
    return TextureManager::Instance()->LoadTexture(path);
  }
  ORB_SPEC void ORB_API SetActiveTexture(ORB_texture t)
  {
    active->SetActiveTexture(t);
  }
  ORB_SPEC void ORB_API DeleteTexture(ORB_texture t)
  {
    TextureManager::Instance()->DropTexture(t);
  }
  ORB_SPEC Vector2D ORB_API GetTextureDimension(ORB_texture t)
  {
    return {(float)t->Width(), (float)t->Height()};
  }
  ORB_SPEC void ORB_API SetUV(glm::mat4 const &uv)
  {

    active->WriteUniform("texMulti", (void *)&uv);
  }
  ORB_SPEC void ORB_API SetTextureSampleMode(ORB_texture t, SAMPLE_SCALE_MODE ssm)
  {
    t->SetSampleMode(static_cast<int>(ssm));
  }
  ORB_SPEC void ORB_API SetUV(Vector2D const &uv, Vector2D const &scale)
  {
    glm::mat4 _frameMatrix = glm::mat4x4(1.0f);
    _frameMatrix = glm::translate(
        _frameMatrix, glm::vec3(uv.x, uv.y, 0.0f));
    _frameMatrix = glm::scale(_frameMatrix, glm::vec3(scale.x, scale.y, 0));
    SetUV(_frameMatrix);
  }
  ORB_SPEC void ORB_API SetUV(float u, float v, float w, float h)
  {
    SetUV({u, v}, {w, h});
  }
  ORB_SPEC void ORB_API BeginMesh()
  {
    _activeMesh = MeshLibrary::Instance()->CreateMesh();
  }
  ORB_SPEC void ORB_API BeginTexMesh()
  {
    _activeMesh = MeshLibrary::Instance()->CreateTexMesh();
  }
  ORB_SPEC void ORB_API MeshSetDrawMode(int mode)
  {
    _activeMesh->DrawMode() = mode;
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos)
  {
    orb::MeshAddVertex(pos, {1, 1, 1, 1}, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector4D color)
  {
    orb::MeshAddVertex(pos, color, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector3D color)
  {
    orb::MeshAddVertex(pos, {color.x, color.y, color.z, 1}, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector4D color, Vector2D UV)
  {
    if (!_activeMesh)
      return;
    _activeMesh->AddVertex({{pos.x, pos.y, 0, 1}, {color.r, color.g, color.b, color.a}, {0, 0, 0, 0}, {UV.x, UV.y}});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos)
  {
    orb::MeshAddVertex(pos, {1, 1, 1, 1}, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color)
  {
    orb::MeshAddVertex(pos, color, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector3D color)
  {
    orb::MeshAddVertex(pos, {color.x, color.y, color.z, 1}, {0, 0});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV)
  {
    if (!_activeMesh)
      return;
    _activeMesh->AddVertex({{pos.x, pos.y, pos.z, 1}, {color.r, color.g, color.b, color.a}, {0, 0, 0, 0}, {UV.x, UV.y}});
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV, Vector4D norm)
  {
    _activeMesh->AddVertex({{pos.x, pos.y, pos.z, 1}, {color.r, color.g, color.b, color.a}, norm, {UV.x, UV.y}});
  }
  ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color)
  {
    _activeMesh->Color() = {color.r, color.g, color.b, color.a};
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(ORB_texture t)
  {
    if (dynamic_cast<TexturedMesh *>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh *>(_activeMesh)->SetTexture(t);
    }
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(std::string &s)
  {
    if (dynamic_cast<TexturedMesh *>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh *>(_activeMesh)->LoadTexture(s);
    }
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(const char *s)
  {
    if (dynamic_cast<TexturedMesh *>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh *>(_activeMesh)->LoadTexture(s);
    }
  }
  ORB_SPEC ORB_mesh ORB_API EndMesh()
  {
    ORB_mesh m = _activeMesh;
    const_cast<ORB_Mesh *>(m)->EndMesh();
    _activeMesh = nullptr;
    return m;
  }
  ORB_SPEC ORB_mesh ORB_API LoadMesh(const char *path)
  {
    ORB_mesh m = MeshLibrary::Instance()->CreateMesh(path);
    _activeMesh = (ORB_Mesh *)m;
    return m;
  }
  ORB_SPEC ORB_mesh ORB_API LoadMesh(std::string &path)
  {
    ORB_mesh m = MeshLibrary::Instance()->CreateMesh(path);
    _activeMesh = (ORB_Mesh *)m;
    return m;
  }

  ORB_SPEC ORB_mesh ORB_API LoadTexMesh(const char *c)
  {
    return MeshLibrary::Instance()->CreateTexMesh(c);
  }

  ORB_SPEC ORB_mesh ORB_API LoadTexMesh(std::string &s)
  {
    return MeshLibrary::Instance()->CreateTexMesh(s);
  }

  ORB_SPEC void ORB_API DrawMesh(const ORB_mesh m, Vector3D const &pos, Vector3D const &scale, Vector3D const &rot, int layer)
  {
    if (!m)
    {
      std::cerr << "ORB ERROR: Attempted to draw non existant mesh" << std::endl;
      return;
    }
    m->Execute();

    active->SetMatrix({pos.x, pos.y, pos.z}, {scale.x, scale.y, scale.z}, {rot.x, rot.y, rot.z});
    if (m->Color() != glm::vec4(1, 1, 1, 1))
      active->SetColor(m->Color());
    active->DrawMesh((*m), (layer >= 0 ? layer : UINT_MAX - layer + 1));
    SetUV(glm::identity<glm::mat4>());
  }

  ORB_SPEC void ORB_API DrawMesh(ORB_mesh m, glm::mat4 matrix, int layer)
  {
    active->SetMatrix(matrix);
    active->DrawMesh((*m), layer);
    SetUV(glm::identity<glm::mat4>());
  }

  ORB_SPEC void ORB_API MeshSetLayer(ORB_mesh m, int l)
  {
    const_cast<ORB_Mesh *>(m)->renderLayer = l;
  }

  ORB_SPEC ORB_font ORB_API LoadFont(const char *path)
  {
    ORB_font f = Fonts::Instance()->LoadFont(path);
    if (f == nullptr)
    {
      errorState = Errors::FontLoadFailure;
      return nullptr;
    }
    return f;
  }

  ORB_SPEC void ORB_API DestroyFont(ORB_font fon)
  {
    Fonts::Instance()->DeleteFont(const_cast<ORB_FontInfo *>(fon));
  }

  ORB_SPEC void ORB_API SetActiveFont(ORB_font f)
  {
    active->SetActiveFont(f);
  }

  ORB_SPEC ORB_texture ORB_API RenderTextToTexture(const char *text, int size, Vector4D const &color)
  {
    return active->RenderText(text, Convert(color), size);
  }

  ORB_SPEC void ORB_API WriteText(const char *text, Vector2D const &pos, int size, Vector4D const &color, int layer)
  {
    ORB_Texture *tex = active->RenderText(text, Convert(color), size);
    auto siz = Fonts::Instance()->MeasureText(const_cast<ORB_FontInfo *>(active->ActiveFont()), text);
    active->SetActiveTexture(tex);
    active->DrawRect(Convert(pos), siz, 0, layer);
    active->SetActiveTexture(nullptr);
    SetUV(glm::identity<glm::mat4>());
  }

  ORB_SPEC void ORB_API LoadCustomRenderPass(std::string const &path)
  {
    if (path.find(".rpass.meta") == std::string::npos)
      return;
    active->LoadRenderPass(path.c_str());
  }

  ORB_SPEC void ORB_API LoadCustomRenderPass(const char *path)
  {
    LoadCustomRenderPass(std::string(path));
  }

  ORB_SPEC void ORB_API SetBufferBase(std::string &buffer, int base)
  {
    active->SetBufferBase(buffer, base);
  }

  ORB_SPEC void ORB_API SetBufferBase(const char *c, int base)
  {
    std::string s = std::string(c);
    active->SetBufferBase(s, base);
  }

  ORB_SPEC void ORB_API WriteBuffer(std::string &buffer, size_t dataSize, void *data)
  {
    active->WriteBuffer(buffer, dataSize, data);
  }

  ORB_SPEC void ORB_API WriteBuffer(const char *buffer, size_t dataSize, void *data)
  {
    std::string s = std::string(buffer);
    active->WriteBuffer(s, dataSize, data);
  }
  ORB_SPEC void ORB_API WriteUniform(std::string &buffer, void *data)
  {
    active->WriteUniform(buffer, data);
  }
  ORB_SPEC void ORB_API WriteUniform(const char *buffer, void *data)
  {
    std::string s = std::string(buffer);
    active->WriteUniform(s, data);
  }
  ORB_SPEC void ORB_API DispatchCompute(int x, int y, int z)
  {
    active->DispatchCompute(x, y, z);
  }

  ORB_SPEC void ORB_API WriteSubBufferData(std::string &buffer, int index, size_t structSize, void *data)
  {
    active->WriteSubBufferData(buffer, index, structSize, data);
  }
  ORB_SPEC void ORB_API WriteSubBufferData(const char *buffer, int index, size_t structSize, void *data)
  {
    std::string s = buffer;
    active->WriteSubBufferData(s, index, structSize, data);
  }
  ORB_SPEC void ORB_API WriteRenderConstantsHere()
  {
    active->WriteRenderConstantsHere();
  }
  ORB_SPEC void ORB_API SetBindTextureToUnit(ORB_texture tex, int unit)
  {
    active->BindTextureToUnit(tex, unit);
  }
  ORB_SPEC void ORB_API DrawIndexed(ORB_mesh m, int count)
  {
    active->DrawIndexed((*m), count);
  }
  ORB_SPEC ORB_FBO ORB_API GetFBOByName(const char *name)
  {
    std::string s = std::string(name);
    auto r = active->GetFBOByName(s);
    return {r.fbo, r.texture};
  }
  ORB_SPEC ORB_FBO ORB_API GetFBOByName(std::string &name)
  {
    auto r = active->GetFBOByName(name);
    return {r.fbo, r.texture};
  }
  ORB_SPEC void ORB_API BindActiveFBO(ORB_FBO of)
  {
    active->BindActiveFBO({of.fbo, of.texture});
  }
  ORB_SPEC void ORB_API ClearFBO(ORB_FBO of)
  {
    active->ClearFBO({of.fbo, of.texture});
  }
  ORB_SPEC void ORB_API SetFBOTextureActive(ORB_FBO f, int binding)
  {
    active->BindTextureToUnit(f.texture, binding);
  }
  ORB_SPEC void ORB_API SetBlendMode(BLEND_MODE b)
  {
    active->SetBlendMode(static_cast<int>(b));
  }
  ORB_SPEC void ORB_API DumpMesh(ORB_mesh m)
  {
    m->Dump();
  }
}
#ifdef __cplusplus
Vector2D::Vector2D(float _x, float _y) : x(_x), y(_y) {}
Vector2D::Vector2D(Vector3D const &a) : x(a.x), y(a.y) {}
Vector2D::Vector2D(Vector4D const &a) : x(a.r), y(a.g) {}
Vector2D::Vector2D(glm::vec2 const &v) : x(v.x), y(v.y) {}
Vector2D::operator glm::vec2() { return glm::vec2(x, y); }

Vector3D::Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vector3D::Vector3D(float _x, float _y) : x(_x), y(_y), z(0){};
Vector3D::Vector3D(Vector2D const &a) : x(a.x), y(a.y), z(0) {}
Vector3D::Vector3D(Vector4D const &a) : x(a.r), y(a.g), z(a.b) {}
Vector3D::Vector3D(glm::vec3 const &v) : x(v.x), y(v.y), z(v.z) {}
Vector3D::operator glm::vec3() { return glm::vec3(x, y, z); }

Vector4D::Vector4D(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
Vector4D::Vector4D(Vector2D const &a) : r(a.x), g(a.y), b(1), a(1) {}
Vector4D::Vector4D(Vector3D const &a) : r(a.x), g(a.y), b(a.z), a(1) {}
Vector4D::Vector4D(glm::vec4 const &v) : r(v.r), g(v.g), b(v.b), a(v.a) {}
Vector4D::operator glm::vec4() { return glm::vec4(r, g, b, a); }
#endif
#ifdef __cplusplus
extern "C"
{
#endif
  ORB_SPEC void ORB_API Initialize()
  {
    orb::Initialize();
  }

  ORB_SPEC void ORB_API RequestAPI(API_VERSION v)
  {
    orb::RequestAPI(v);
  }

  ORB_SPEC void ORB_API EnableDebugOutput(bool b)
  {
    orb::EnableDebugOutput(b);
  }

  ORB_SPEC void Update()
  {
    orb::Update();
  }

  ORB_SPEC void EnableLighting(bool value)
  {
    orb::EnableLighting(value);
  }

  ORB_SPEC void EnableShadows(bool b)
  {
    orb::EnableShadows(b);
  }

  ORB_SPEC void SetMaterialProperties(Vector3D diffuse, Vector3D specular, float specular_exponent)
  {
    orb::SetMaterialProperties(diffuse, specular, specular_exponent);
  }

  ORB_SPEC void SetMaterial(int id)
  {
    orb::SetMaterial(id);
  }

  ORB_SPEC void SetLight(Vector4D pos, Vector3D color)
  {
    orb::SetLight(pos, color);
  }

  ORB_SPEC void EnableStoredRender(bool b)
  {
    orb::EnableStoredRender(b);
  }

  ORB_SPEC void ORB_API RegisterRenderCallback(int (*Callback)(), RENDER_STAGE stage, int index)
  {
    orb::RegisterRenderCallback(Callback, stage, index);
  }

  ORB_SPEC void ORB_API RegisterKeyboardCallback(KeyCallback callback)
  {
    orb::RegisterKeyboardCallback(callback);
  }

  ORB_SPEC void ORB_API RegisterMouseButtonCallback(MouseButtonCallback callback)
  {
    orb::RegisterMouseButtonCallback(callback);
  }

  ORB_SPEC void ORB_API RegisterMouseMovementCallback(MouseMovmentCallback callback)
  {
    orb::RegisterMouseMovementCallback(callback);
  }

  ORB_SPEC void ORB_API RegisterWindowCallback(WindowCallback callback)
  {
    orb::RegisterWindowCallback(callback);
  }

  ORB_SPEC void ORB_API RegisterMessageCallback(GenericCallback callback)
  {
    orb::RegisterMessageCallback(callback);
  }

  ORB_SPEC void ORB_API ShutDown()
  {
    orb::ShutDown();
  }

  ORB_SPEC int ORB_API GetError()
  {
    return orb::GetError();
  }

  ORB_SPEC int ORB_API IsRunning()
  {
    return static_cast<int>(orb::IsRunning());
  }

  ORB_SPEC Vector2D ORB_API ToScreenSpace(Vector2D v)
  {
    return orb::ToScreenSpace(v);
  }

  ORB_SPEC Vector2D ORB_API ToWorldSpace(Vector2D v)
  {
    return orb::ToWorldSpace(v);
  }

  ORB_SPEC Window *CreateNewWindow(const char *c)
  {
    std::string s = std::string(c);
    return orb::CreateNewWindow(s);
  }

  ORB_SPEC void ORBActiveWindow(Window *w)
  {
    orb::SetActiveWindow(w);
  }

  ORB_SPEC Window *RetrieveWindow(int index)
  {
    return orb::RetrieveWindow(index);
  }

  ORB_SPEC void ORB_API SetWindowClearColor(Window *w, Vector4D color)
  {
    orb::SetWindowClearColor(w, color);
  }

  ORB_SPEC void ORB_API SetWindowPosition(Window *w, int x, int y)
  {
    orb::SetWindowPosition(w, x, y);
  }

  ORB_SPEC void ORB_API SetWindowScale(Window *wi, int w, int h)
  {
    orb::SetWindowScale(wi, w, h);
  }

  ORB_SPEC void ORB_API SetWindowViewport(Window *w, int vx, int vy, int vw, int vh)
  {
    orb::SetWindowViewport(w, vx, vy, vw, vh);
  }

  ORB_SPEC void ORB_API SetWindowMax(Window *w)
  {
    orb::SetWindowMax(w);
  }

  ORB_SPEC void ORB_API SetWindowFullScreen(Window *w, int type)
  {
    orb::SetWindowFullScreen(w, type);
  }

  ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale, int layer)
  {
    orb::DrawRect(pos, scale, layer);
  }

  ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation, int layer)
  {
    orb::DrawRectAdvanced(pos, scale, rotation, layer);
  }

  ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a)
  {
    orb::SetDrawColor(r, g, b, a);
  }

  ORB_SPEC void ORB_API DrawLine(Vector3D start, Vector3D end, int depth)
  {
    orb::DrawLine(start, end, depth);
  }

  ORB_SPEC void ORB_API SetProjectionMode(PROJECTION_TYPE p)
  {
    orb::SetProjectionMode(p);
  }

  ORB_SPEC void ORB_API SetDefaultRenderMode(int i)
  {
    orb::SetDefaultRenderMode(i);
  }

  ORB_SPEC void ORB_API SetFillMode(int f)
  {
    orb::SetFillMode(f);
  }

  ORB_SPEC void ORB_API SetZoom(float z)
  {
    orb::SetZoom(z);
  }

  ORB_SPEC float ORB_API GetZoom()
  {
    return orb::GetZoom();
  }

  ORB_SPEC Vector2D ORB_API GetWindowSize(Window *w)
  {
    return orb::GetWindowSize(w);
  }

  ORB_SPEC Vector2D ORB_API GetCameraPosition()
  {
    return orb::GetCameraPosition();
  }

  ORB_SPEC void ORB_API SetCameraPosition(Vector2D pos)
  {
    orb::SetCameraPosition(pos);
  }

  ORB_SPEC void ORB_API SetCameraRotation(Vector3D rot)
  {
    orb::SetCameraRotation(rot);
  }

  ORB_SPEC ORB_texture ORB_API LoadTexture(const char *path)
  {
    return orb::LoadTexture(path);
  }

  ORB_SPEC void ORB_API SetActiveTexture(ORB_texture t)
  {
    orb::SetActiveTexture(t);
  }

  ORB_SPEC void ORB_API DeleteTexture(ORB_texture t)
  {
    orb::DeleteTexture(t);
  }

  ORB_SPEC Vector2D ORB_API GetTextureDimension(ORB_texture t)
  {
    return orb::GetTextureDimension(t);
  }

  ORB_SPEC void ORB_API SetUV(Vector2D const *center, Vector2D const *scale)
  {
    orb::SetUV(*center, *scale);
  }

  ORB_SPEC void ORB_API SetTextureSampleMode(ORB_texture t, enum SAMPLE_SCALE_MODE ssm)
  {
    orb::SetTextureSampleMode(t, ssm);
  }

  ORB_SPEC void ORB_API BeginMesh()
  {
    orb::BeginMesh();
  }

  ORB_SPEC void ORB_API BeginTexMesh()
  {
    orb::BeginTexMesh();
  }

  ORB_SPEC void ORB_API MeshSetDrawMode(int mode)
  {
    orb::MeshSetDrawMode(mode);
  }

  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV)
  {
    orb::MeshAddVertex(pos, color, UV);
  }

  ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color)
  {
    orb::MeshSetDrawColor(color);
  }

  ORB_SPEC void ORB_API TexMeshSetTextureFromPointer(ORB_texture t)
  {
    orb::TexMeshSetTexture(t);
  }

  ORB_SPEC void ORB_API TexMeshSetTextureFromString(const char *s)
  {
    return orb::TexMeshSetTexture(s);
  }

  ORB_SPEC ORB_mesh ORB_API EndMesh()
  {
    return orb::EndMesh();
  }

  ORB_SPEC ORB_mesh ORB_API LoadMesh(const char *c)
  {
    return orb::LoadMesh(c);
  }

  ORB_SPEC ORB_mesh ORB_API LoadTexMesh(const char *c)
  {
    return orb::LoadTexMesh(c);
  }

  ORB_SPEC void ORB_API DrawMesh(ORB_mesh m, Vector3D const *pos, Vector3D const *scale, Vector3D const *rot, int layer)
  {
    orb::DrawMesh(m, *pos, *scale, *rot, layer);
  }

  ORB_SPEC void ORB_API MeshSetLayer(ORB_mesh m, int l)
  {
    orb::MeshSetLayer(m, l);
  }

  ORB_SPEC ORB_font ORB_API LoadFont(const char *path)
  {
    return orb::LoadFont(path);
  }

  ORB_SPEC void ORB_API DestroyFont(ORB_font font)
  {
    orb::DestroyFont(font);
  }

  ORB_SPEC void ORB_API SetActiveFont(ORB_font f)
  {
    orb::SetActiveFont(f);
  }

  ORB_SPEC ORB_texture ORB_API RenderTextToTexture(const char *text, int size, Vector4D const *color)
  {
    return orb::RenderTextToTexture(text, size, *color);
  }

  ORB_SPEC void ORB_API WriteText(const char *text, Vector2D const *pos, int size, Vector4D const *color, int layer)
  {
    orb::WriteText(text, *pos, size, *color, layer);
  }
  ORB_SPEC void ORB_API DumpMesh(ORB_mesh m)
  {
    m->Dump();
  }
#ifdef __cplusplus
}
#endif