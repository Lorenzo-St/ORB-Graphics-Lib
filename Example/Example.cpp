// Example.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #define ORB_EXPOSE_GLM
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <iostream>
#include "../OverloadedRenderBackend/OverloadedRenderBackend.h"
#include <assert.h>
#include <cmath>
#include <bit>
ORB_texture image;
ORB_mesh m;
ORB_mesh tex;
ORB_mesh temp;
Window *w;

Vector2D boxPos;
Vector2D mPosA;
ORB_font f;
float x = 0, y = 0;
int callbackTest()
{
  orb::SetActiveWindow(w);

  orb::SetDrawColor(255, 255, 255, 255);
  orb::SetActiveTexture(image);
  orb::DrawRect(boxPos.x, boxPos.y, 100, 100);

  orb::SetActiveTexture(nullptr);
  orb::DrawMesh(m, {2.0f, 1.0f, -1750}, {10.0f, 10.0f}, {});
  orb::SetDrawColor(0, 255, 255, 255);

  orb::DrawMesh(temp, {-200.0f, 1.0f, -1750}, {100.0f, 100.0f}, {});
  orb::SetDrawColor(125, 125, 255, 255);
  orb::DrawMesh(tex, {100.0f, 1.0f, -1750}, {100.0f, 100.0f}, {});
  orb::SetDrawColor(255, 0, 255, 255);
  orb::DrawMesh(temp, {-200.0f, 1.0f, -1750}, {150.0f, 125.0f}, {});
  orb::SetDrawColor(255, 125, 125, 255);
  orb::DrawMesh(tex, {mPosA.x, mPosA.y, 0}, {200.0f, 200.0f}, {});
  orb::DrawLine(Vector2D(0, 0), {100, 100});
  orb::SetActiveTexture(nullptr);
  orb::SetDrawColor(255, 255, 100, 120);

  orb::DrawRect(mPosA.x, 1, 120, 400);
  orb::SetDrawColor(255, 30, 200, 255);

  orb::WriteText("Hello", {10, 10}, 100, {1, 1, 1, 1});

  orb::SetDrawColor(0, 0, 0, 255);
  orb::DrawRect({-x, -y}, {50, 50}, 1);
  orb::SetDrawColor(255, 255, 255, 255);
  orb::DrawRect(orb::GetCameraPosition(), {50, 50}, 2);
  orb::DrawIndexed(m, 100);
  // orb::SetCameraPosition({ -boxPos.x , -boxPos.y });
  return 0;
}

void MousePosTest(int x, int y, int, int)
{
  mPosA = orb::ToWorldSpace({(float)x, (float)y});
}
void MouseButton(MOUSEBUTTON m, KEY_STATE k)
{
  std::cout << static_cast<int>(m) << " " << static_cast<int>(k) << std::endl;
}

void KeyInput(uchar key, KEY_STATE state)
{
  // std::cout << "Key Callback Triggered: Key=" << key << " State=" << static_cast<int>(state) << std::endl;

  if (state == KEY_STATE::PRESSED || state == KEY_STATE::HELD)
  {
    switch (key)
    {
    case 'a':

      boxPos.x -= 10;
      break;
    case 'd':
      boxPos.x += 10;
      break;
    case 'w':
      boxPos.y += 10;
      break;
    case 's':
      boxPos.y -= 10;
      break;
    }
  }
}

int main()
{
  if constexpr (std::endian::native == std::endian::little)
  {

    std::cout << "little" << std::endl;
  }
  else if constexpr (std::endian::native == std::endian::big)
  {

    std::cout << "bit" << std::endl;
  }

  orb::Initialize();
  orb::EnableDebugOutput(false);
  orb::SetProjectionMode(PROJECTION_TYPE::PERSPECTIVE);
  w = orb::RetrieveWindow();
  orb::SetActiveWindow(w);
  orb::EnableStoredRender(true);
  orb::EnableLighting(true);
  orb::SetMaterialProperties({.3, .3, .3}, {.7, .7, .7}, .5);
  orb::SetMaterial(0);
  orb::RegisterRenderCallback(callbackTest);
  f = orb::LoadFont("./Adamina-Regular.ttf");
  orb::SetActiveFont(f);
  orb::RegisterKeyboardCallback(KeyInput);
  orb::RegisterMouseMovementCallback(MousePosTest);
  orb::RegisterMouseButtonCallback(MouseButton);
  // orb::SetWindowMax(w);
  image = orb::LoadTexture("./orb.png");
  orb::BeginMesh();

  orb::MeshAddVertex({-.5f, -.5f}, {1, 1, 1, 1}, {0, 1}, {0, 0, 1, 0});
  orb::MeshAddVertex({.5f, -.5f}, {1, 1, 1, 1}, {1, 1}, {0, 0, 1, 0});
  orb::MeshAddVertex({.5f, .5f}, {1, 1, 1, 1}, {1, 0}, {0, 0, 1, 0});
  orb::MeshAddVertex({-.5f, .5f}, {1, 1, 1, 1}, {0, 0}, {0, 0, 1, 0});

  orb::MeshSetDrawMode(6);

  orb::MeshSetDrawColor({.390625f, 0.196078f, 1.0f, 1.0f});

  m = orb::EndMesh();
  orb::SetWindowClearColor(w, {1, 1, 1, 1});
  temp = orb::LoadMesh("./Circle.dat");
  tex = orb::LoadTexMesh("./TexCircle.dat");
  // orb::SetZoom(3);
  float time = 0;
  while (orb::IsRunning())
  {
    orb::Update();
    x = 500 * std::cos(time * 20);
    y = 500 * std::sin(time * 20);
    time += 1.f / 60.f * .1f;
  }

  orb::ShutDown();
}
