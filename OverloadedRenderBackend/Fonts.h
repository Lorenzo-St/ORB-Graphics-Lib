/*********************************************************************
 * @file   Fonts.h
 * @brief  Holds the class information for Fonts
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   September 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once
#define SDL_MAIN_HANDLED
#include "SDL_ttf.h"
#include "glm.hpp"
#include <string>
#include <vector>

typedef struct FontInfo
{
    TTF_Font* font;
    const char* name;
} FontInfo;

class Fonts
{
public:
    /**
     * @brief Destructor.
     *
     */
    ~Fonts();
    /**
     * @brief Load a font with default size.
     *
     * @param c the filename
     * @return the font that was loaded
     */
    FontInfo* LoadFont(const char* c);
    /**
     * @brief Load a a font with custom size.
     *
     * @param c the filename
     * @param fontsize size to load the font as
     * @return the font
     */
    FontInfo* LoadFont(const char* c, int fontsize);
    /**
     * @brief Get a font from the array.
     *
     * @param name the filename
     * @return the found font, nullptr if font wasn't found
     */
    FontInfo* FetchFont(const char* name);
    /**
     * @brief Delete all fonts from memory.
     *
     */
    void DeleteFonts();
    /**
     * @brief Delete a font.
     * @param font - the FontInfo* to delete
     */
    void DeleteFont(FontInfo* f);
    /**
     * @brief Measures text.
     *
     * @param text the text to measure
     * @return the scale of the text needed
     */
    glm::vec2 MeasureText(FontInfo* f, const char* text);
    
    static Fonts* Instance();
private:
    /**
     * @brief Ctor.
     */
    Fonts();
    std::vector<FontInfo*> _fonts;
    static inline Fonts* _instance;
};