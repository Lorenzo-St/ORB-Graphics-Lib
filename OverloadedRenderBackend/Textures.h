/*********************************************************************
 * @file   Textures.h
 * @brief  Holds the class definitions for things realted to textures
 * @author Lorenzo St. Luce (lorenzo.stluce)
 * @date   September 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once
#include "ShaderLog.hpp"
#include "glad.h"
#include <string>
#include <vector>
typedef GLuint Image;

typedef struct ORB_Texture
{
public:
    friend class TextureManager;
    /**
     * @brief Create a new Texture object.
     *
     * @param te the image name to associate with this texture
     * @param wi the width of the texture
     * @param he the height of the texture
     */
    ORB_Texture(Image te, int wi, int he, GLenum format, bool keepAlive)
        : _texture(te), _w(wi), _h(he), _uses(0),  _keepAlive(keepAlive), _format(format)
    {
    }
    /**
     * @brief Return the image name of the texture.
     *
     * @return The image name
     */
    Image const texture() const;
    /**
     * @brief Return the file name of the texture.
     *
     * @return the file name
     */
    std::string const& name() const;
    /**
     * @brief Set the file name of a texture.
     *
     * @param s the new file name
     */
    void name(std::string const& s);
    /**
     * @brief Get the width of a texture.
     *
     * @return the width
     */
    int Width() const;
    /**
     * @brief Get the height of a texture.
     *
     * @return the height
     */
    int Height() const;

    GLenum Format() const;

    /**
     * @brief Increase the uses count for the texture
     *
     */
    void IncramentUses();

    void SetSampleMode(int mode);

private:
    std::string _name;
    Image _texture;
    int _w, _h, _uses;
    bool _keepAlive;
    GLenum _format;
} Texture;

class TextureManager
{
public:

    /**
     * @brief .Load a texture
     *
     * @param filename std::string of filename
     * @return the loaded texture
     */
    ORB_Texture* LoadTexture(std::string filename, bool KeepAlive = true);
    /**
     * @brief Load a texutre.
     *
     * @param filename const char* to filename
     * @return the loaded texutre
     */
    ORB_Texture* LoadTexture(const char* filename);
    /**
     * @brief Create a texture from program memory.
     *
     * @param name the texture filename
     * @param w the width of the texture
     * @param h the height of the texture
     * @param depth the color count of the texture
     * @details depth can be 3 or 4 with 3 being RGB and 4 being RGBA
     * @param data pointer to the data to write to teh texture
     * @return the texture created. NOTE: To delete the texture later you must use
     * DeleteTextureFromMemory
     */
    ORB_Texture* CreateFromMemeory(std::string name, int w, int h, int depth, void* data);
    /**
     * @brief Deletes a texture made in memory.
     *
     * @param t the texture
     */
    void DeleteTextureFromMemory(ORB_Texture* t);
    /**
     * @brief Upate the manager
     *
     */
    void Update(void);
    /**
     * @brief Delete a texture.
     *
     * @param t the texture to delete
     */
    void DropTexture(ORB_Texture* t);
    /**
     * @brief Delete all textures.
     *
     */
    void DropAll(void);


    TraceLevels _globalTraceLevel = TraceLevels::Extra;

    /**
     * @brief Log to the console.
     *
     * @param l the Tracelevel to log at
     * @param arg1 the first arg
     * @param variadic the rest of the args
     */
    template <typename Arg, typename... vArgs>
    void Log(TraceLevels l, Arg&& arg1, vArgs&&... variadic);

    std::vector<ORB_Texture*> const& GetTextures() const;
    static TextureManager* Instance();
private:
  TextureManager();
  /**
   * @brief Destruct the TexturesManager.
   *
   */
  ~TextureManager();
    void checkError();
    std::vector<ORB_Texture*> _textures;
    static inline TextureManager* _instance;
};

