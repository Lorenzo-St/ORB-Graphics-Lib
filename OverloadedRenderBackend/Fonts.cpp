/*********************************************************************
 * @file   Fonts.cpp
 * @brief  Holds the impementation for the fonts class
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   September 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#include "pch.h"

#include "Fonts.h"

inline Fonts::~Fonts()
{
    TTF_Quit();
    SDL_Quit();
}

FontInfo* Fonts::LoadFont(const char* c)
{
    return LoadFont(c, 24);
}

FontInfo* Fonts::LoadFont(const char* c, int size)
{

    if (_instance == nullptr)
        _instance = new Fonts();
    FontInfo* f = new FontInfo();
    f->font = TTF_OpenFont(c, size);
    if (f->font == nullptr)
        throw std::runtime_error("Failed to create font");
    f->name = c;
    _instance->_fonts.push_back(f);
    return _instance->_fonts[_instance->_fonts.size() - 1];
}

FontInfo* Fonts::FetchFont(const char* name)
{
    if (_instance == nullptr)
    {
        _instance = new Fonts();
        return nullptr;
    }
    for (auto const& c : _instance->_fonts)
    {
        if (c->name == name)
            return c;
    }
    return nullptr;
}

void Fonts::DeleteFonts()
{
    if (_instance == nullptr)
    {
        _instance = new Fonts();
        return;
    }
    for (auto& f : _instance->_fonts)
    {
        TTF_CloseFont(f->font);
        f->font = nullptr;
        delete f;
        f = nullptr;
    }
    _instance->_fonts.clear();
}

void Fonts::DeleteFont(FontInfo* f)
{
  TTF_CloseFont(f->font);
  f->font = nullptr;
  auto res = std::find(_fonts.begin(), _fonts.end(), f);
  _fonts.erase(res);
  delete f;
}

/**
 * @brief Ctor.
 */

Fonts::Fonts()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
}

glm::vec2 Fonts::MeasureText(FontInfo* f, const char* text)
{
    if (f == nullptr)
        return {0, 0};
    int width;
    int height;
    TTF_SizeText(f->font, text, &width, &height);

    return {width, height};
}

Fonts* Fonts::Instance()
{
  if (_instance == nullptr)
    _instance = new Fonts();
  return _instance;
}
