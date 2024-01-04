#include "pch.h"
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

Renderer* active = nullptr;
Errors errorState = Errors::NoError;

Mesh* _activeMesh = nullptr;

std::vector<Window*> activeWindows;
Window* defaultWindow = nullptr;

KeyCallback keyFunction;
std::unordered_map<char, KEY_STATE> keys;

MouseButtonCallback mbuttonFunction;
std::unordered_map<int, KEY_STATE> mouseButtons;

MouseMovmentCallback mMovementFunction;


GenericCallback genericFunction;

void LogError(const char* s)
{
  std::cerr << "ORB ERROR: " << s << std::endl;
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
  return { v.x, v.y };
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
  }

  ORB_SPEC void ORB_API  RequestAPI(API_VERSION version)
  {

  }

  void CallKeyCallBack(char key, KEY_STATE newstate)
  {
    if (keyFunction != nullptr)
    {
      KEY_STATE& state = keys[key];
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


  ORB_SPEC void ORB_API Update()
  {
    active->Update();
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
    {
      if (genericFunction != nullptr)
        genericFunction(&ev);
      switch (ev.type)
      {
      case SDL_WINDOWEVENT:

        switch (ev.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
        {
          auto w = SDL_GetWindowFromID(ev.window.windowID);
          auto fin = [w](Window* wi) {return wi->window == w; };
          auto toClose = std::find_if(activeWindows.begin(), activeWindows.end(), fin);
          if (toClose != activeWindows.end())
          {
            active->DestroyWindow(*toClose);
          }

        }
        break;
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
        //std::cout << ev.key.keysym.sym << " " << +ev.key.state << std::endl;
        if (ev.key.state > 1)
          ev.key.state = 1;
        CallKeyCallBack(ev.key.keysym.sym, static_cast<KEY_STATE>(ev.key.state));
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        if (mbuttonFunction != nullptr)
        {
          auto b = ev.button.button;
          KEY_STATE newstate = static_cast<KEY_STATE>(ev.button.state);
          if (keyFunction != nullptr)
          {
            KEY_STATE& state = mouseButtons[b];
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

  ORB_SPEC Window* CreateNewWindow()
  {
    Window* w = active->MakeWindow();
    activeWindows.push_back(w);
    return w;
  }

  ORB_SPEC Window* CreateNewWindow(std::string name)
  {
    Window* w = active->MakeWindow(name);
    activeWindows.push_back(w);
    return w;
  }

  ORB_SPEC void SetActiveWindow(Window* w)
  {
    active->SetActiveWindow(w);

  }

  ORB_SPEC Window* RetrieveWindow(int index)
  {
    if (index < 0 || index >= activeWindows.size())
      return nullptr;
    return activeWindows[index];

  }

  ORB_SPEC void ORB_API SetWindowClearColor(Window* w, Vector4D color)
  {
    w->color = { color.r, color.g, color.b, color.a };
  }

  ORB_SPEC void ORB_API RegisterRenderCallback(int (*Callback)(), RENDER_STAGE  stage, int index)
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

  ORB_SPEC void ORB_API RegisterMessageCallback(GenericCallback callback)
  {
    genericFunction = callback;

  }

  ORB_SPEC void ORB_API  ShutDown()
  {
    try
    {
      for (auto& w : activeWindows)
      {
        active->DestroyWindow(w);
        delete w;
        w = nullptr;
      }
      activeWindows.clear();
      delete active;
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
  ORB_SPEC void ORB_API DrawRect(float x, float y, float width, float height)
  {
    active->DrawRect({ x,y }, { width, height }, 0);
  }
  ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale)
  {
    active->DrawRect({ pos.x, pos.y }, { scale.x, scale.y }, 0);

  }
  ORB_SPEC void ORB_API DrawRectAdvanced(float x, float y, float width, float height, float rotation)
  {
    active->DrawRect({ x,y }, { width, height }, rotation);
  }
  ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation)
  {
    active->DrawRect({ pos.x,pos.y }, { scale.x, scale.y }, rotation);

  }
  ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a)
  {
    active->SetColor({ r / 255.f, g / 255.f, b / 255.f, a / 255.f });
  }
  ORB_SPEC bool ORB_API IsRunning()
  {
    return active->running;
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
  ORB_SPEC texture ORB_API LoadTexture(std::string path)
  {
    return TextureManager::Instance()->LoadTexture(path);
  }
  ORB_SPEC texture ORB_API LoadTexture(const char* path)
  {
    return TextureManager::Instance()->LoadTexture(path);
  }
  ORB_SPEC void ORB_API SetActiveTexture(texture t)
  {
    active->SetActiveTexture(t);
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
    MeshAddVertex(pos, { 1,1,1,1 }, { 0,0 });
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector4D color)
  {
    MeshAddVertex(pos, color, { 0,0 });

  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector3D color)
  {
    MeshAddVertex(pos, { color.x, color.y, color.z, 1 }, { 0,0 });
  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector2D pos, Vector4D color, Vector2D UV)
  {
    if (!_activeMesh) return;
    _activeMesh->AddVertex({ {pos.x, pos.y, 0, 1}, {color.r,color.g,color.b,color.a}, {UV.x,UV.y} });

  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos)
  {
    MeshAddVertex(pos, { 1,1,1,1 }, { 0,0 });

  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color)
  {
    MeshAddVertex(pos, color, { 0,0 });

  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector3D color)
  {
    MeshAddVertex(pos, { color.x,color.y,color.z,1 }, { 0,0 });

  }
  ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV)
  {
    if (!_activeMesh) return;
    _activeMesh->AddVertex({ {pos.x, pos.y, pos.z, 1}, {color.r,color.g,color.b,color.a}, {UV.x,UV.y} });

  }
  ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color)
  {
    _activeMesh->Color() = { color.r, color.g, color.b, color.a };
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(texture t)
  {
    if (dynamic_cast<TexturedMesh*>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh*>(_activeMesh)->SetTexture(t);
    }
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(std::string s)
  {
    if (dynamic_cast<TexturedMesh*>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh*>(_activeMesh)->LoadTexture(s);
    }
  }
  ORB_SPEC void ORB_API TexMeshSetTexture(const char* s)
  {
    if (dynamic_cast<TexturedMesh*>(_activeMesh) != nullptr)
    {
      dynamic_cast<TexturedMesh*>(_activeMesh)->LoadTexture(s);
    }
  }
  ORB_SPEC mesh ORB_API EndMesh()
  {
    mesh m = _activeMesh;
    _activeMesh = nullptr;
    return m;
  }
  ORB_SPEC mesh ORB_API LoadMesh(const char* path)
  {
    return MeshLibrary::Instance()->CreateMesh(path);
  }
  ORB_SPEC mesh ORB_API LoadMesh(std::string path)
  {
    return MeshLibrary::Instance()->CreateMesh(path);

  }

  ORB_SPEC mesh ORB_API LoadTexMesh(const char* c)
  {
    return MeshLibrary::Instance()->CreateTexMesh(c);
  }

  ORB_SPEC mesh ORB_API LoadTexMesh(std::string s)
  {
    return MeshLibrary::Instance()->CreateTexMesh(s);
  }

  ORB_SPEC void ORB_API DrawMesh(const mesh m, Vector3D const& pos, Vector3D const& scale, Vector3D const& rot)
  {
    if (!m)
    {
      std::cerr << "ORB ERROR: Attempted to draw non existant mesh" << std::endl;
      return;
    }
    m->Execute();
    active->SetMatrix({ pos.x, pos.y, pos.z }, { scale.x, scale.y, scale.z }, { rot.x, rot.y, rot.z });
    active->SetColor(m->Color());
    active->DrawMesh(m->Verticies(), 1, m->DrawMode());
  }

  ORB_SPEC font ORB_API LoadFont(const char* path)
  {
    font f = Fonts::Instance()->LoadFont(path);
    if (f == nullptr)
    {
      errorState = Errors::FontLoadFailure;
      return nullptr;
    }
    return f;
  }

  ORB_SPEC void ORB_API DestroyFont(font fon)
  {
    Fonts::Instance()->DeleteFont(const_cast<FontInfo*>(fon));
  }

  ORB_SPEC void ORB_API SetActiveFont(font f)
  {
    active->SetActiveFont(f);
  }

  ORB_SPEC texture ORB_API RenderTextToTexture(const char* text, int size, Vector4D const& color)
  {
    return active->RenderText(text, Convert(color), size);
  }

  ORB_SPEC void ORB_API WriteText(const char* text, Vector2D const& pos, int size, Vector4D const& color)
  {
    Texture* tex = active->RenderText(text, Convert(color), size);
    auto siz = Fonts::Instance()->MeasureText(const_cast<FontInfo*>(active->ActiveFont()), text);
    active->SetActiveTexture(tex);
    active->DrawRect(Convert(pos), siz, 0);
    active->SetActiveTexture(nullptr);

  }

  ORB_SPEC void ORB_API LoadCustomRenderPass(std::string path)
  {
    if (path.find(".rpass.meta") == std::string::npos)
      return;
    active->LoadRenderPass(path);
  }

  ORB_SPEC void ORB_API WriteBuffer(std::string buffer, size_t dataSize, void* data)
  {
    active->WriteBuffer(buffer, dataSize, data);
  }

  ORB_SPEC void ORB_API WriteUniform(std::string buffer, void* data)
  {
    active->WriteUniform(buffer, data);
  }

  ORB_SPEC void ORB_API DispatchCompute(int x, int y, int z)
  {
    active->DispatchCompute(x, y, z);
  }

}
Vector2D::Vector2D(float _x, float _y) : x(_x), y(_y) {}
Vector2D::Vector2D(Vector3D const& a) : x(a.x), y(a.y) {}
Vector2D::Vector2D(Vector4D const& a) : x(a.r), y(a.g) {}

Vector3D::Vector3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vector3D::Vector3D(float _x, float _y) : x(_x), y(_y), z(0) {};
Vector3D::Vector3D(Vector2D const& a) : x(a.x), y(a.y), z(0) {}
Vector3D::Vector3D(Vector4D const& a) : x(a.r), y(a.g), z(a.b) {}

Vector4D::Vector4D(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
Vector4D::Vector4D(Vector2D const& a) : r(a.x), g(a.y), b(1), a(1) {}
Vector4D::Vector4D(Vector3D const& a) : r(a.x), g(a.y), b(a.z), a(1) {}
