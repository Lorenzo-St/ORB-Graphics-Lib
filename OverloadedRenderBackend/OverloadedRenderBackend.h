/*********************************************************************
 * @file   OverloadedRenderBackend.h
 * @brief
 *
 * @author Lorenzo St. Luce
 * @date   December 2023
 *********************************************************************/
#pragma once
#ifdef __GNUC__
  #ifdef __X86__
  #define __cdecl __attribute__((__cdecl__))
  #else 
  #define __cdecl
  #endif
  #define __declspec(p) __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#include <vector>
#include <string>
#define ORB_ENUM enum class
#define ORB_ETYPE(x) : x
#else
#define ORB_ENUM enum 
#define ORB_ETYPE(X)
#define bool int
#endif
#define ORB_API __cdecl

#ifdef ORB_BUILD
#define ORB_SPEC __declspec(dllexport)
#else
#define ORB_SPEC
#endif

#ifdef ORB_EXPOSE_GLM
#define ORB_GLM
#include "glm.hpp"
#endif




typedef ORB_ENUM API_VERSION ORB_ETYPE(int)
{
  OPENGL_LATEST,
    OPENGL_450,
    OPENGL_430,
    VULKAN,
    DIRECTX
}API_VERSION;



typedef ORB_ENUM RENDER_STAGE ORB_ETYPE(int)
{
  PRERENDER,
    PRIMARY_RENDER,
    SECONDARY_RENDER,
    POST_RENDER,
    PRE_FRAME_SWAP,
    POST_FRAME_SWAP
}RENDER_STAGE;

typedef ORB_ENUM PROJECTION_TYPE ORB_ETYPE(int)
{
  ORTHOGONAL,
    PERSPECTIVE
}PROJECTION_TYPE;

typedef ORB_ENUM KEY_STATE ORB_ETYPE(int)
{
  INACTIVE = -1,
    RELEASED = 0,
    PRESSED = 1,
    HELD = 2,
}KEY_STATE;

typedef ORB_ENUM MOUSEBUTTON ORB_ETYPE(int)
{
  LEFT = 1,
    RIGHT = 2,
    MIDDLE = 3
}MOUSEBUTTON;

typedef ORB_ENUM BLEND_MODE ORB_ETYPE(int) 
{
  OFF,
  ADD,
  SUBTRACT,
  INVERTED_SUBTRACT,
  MIN,
  MAX,
}BLEND_MODE;

typedef ORB_ENUM NONPRINTINGKEYS ORB_ETYPE(int)
{
  KEY_UP = 72,
    KEY_DOWN = 80,
    KEY_LEFT = 75,
    KEY_RIGHT = 77,
    F0 = 157,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    LEFT_SHIFT = 225,
    RIGHT_SHIFT = 229,
}NONPRINTINGKEYS;

typedef ORB_ENUM SAMPLE_SCALE_MODE ORB_ETYPE(int)
{
  linear,
  nearest,

}SAMPLE_MODE;


typedef struct Window Window;

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct ORB_Texture ORB_Texture;
typedef ORB_Texture* ORB_texture;

typedef struct ORB_Mesh ORB_Mesh;
typedef ORB_Mesh const* ORB_mesh;

typedef struct ORB_FontInfo ORB_FontInfo;
typedef ORB_FontInfo const* ORB_font;

// SDL Forward declarations
typedef union SDL_Event SDL_Event;
typedef SDL_Event* ORB_Event;
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

typedef struct ORB_FBO {
  uint fbo;
  uint texture;
}ORB_FBO;

typedef void(*KeyCallback)(uchar key, KEY_STATE state);
typedef void(*MouseButtonCallback)(MOUSEBUTTON button, KEY_STATE state);
typedef void(*MouseMovmentCallback)(int x, int y, int deltaX, int deltaY);
typedef void(*WindowCallback)(Window* w, int x, int y, int wi, int he, bool focused);
typedef bool (*GenericCallback)(ORB_Event);

typedef struct Vector4D Vector4D;
typedef struct Vector3D Vector3D;
#ifdef __cplusplus
extern "C" {
#endif
  typedef struct Vector2D
  {
    float x;
    float y;
#ifdef __cplusplus
    ORB_SPEC ORB_API Vector2D() = default;
    ORB_SPEC ORB_API  Vector2D(float _x, float _y);
    ORB_SPEC ORB_API  Vector2D(Vector3D const& a);
    ORB_SPEC ORB_API  Vector2D(Vector4D const& a);
#ifdef ORB_GLM
    ORB_SPEC ORB_API Vector2D(glm::vec2 const&);
    ORB_SPEC  ORB_API operator glm::vec2();
#endif
#endif

  }Vector2D;
  typedef struct Vector3D
  {
    float x;
    float y;
    float z;
#ifdef __cplusplus
    ORB_SPEC ORB_API Vector3D() = default;
    ORB_SPEC ORB_API Vector3D(float _x, float _y, float _z);
    ORB_SPEC ORB_API Vector3D(float _x, float _y);

    ORB_SPEC ORB_API Vector3D(Vector2D const& a);
    ORB_SPEC ORB_API Vector3D(Vector4D const& a);
#ifdef ORB_GLM
    ORB_SPEC ORB_API Vector3D(glm::vec3 const&);
    ORB_SPEC ORB_API operator glm::vec3();
#endif
#endif
  }Vector3D;
  typedef struct Vector4D
  {
    float r;
    float g;
    float b;
    float a;
#ifdef __cplusplus
    ORB_SPEC ORB_API Vector4D() = default;
    ORB_SPEC ORB_API Vector4D(float _r, float _g, float _b, float _a);
    ORB_SPEC ORB_API Vector4D(Vector2D const& a);
    ORB_SPEC ORB_API Vector4D(Vector3D const& a);
#ifdef ORB_GLM
    ORB_SPEC ORB_API Vector4D(glm::vec4 const&);
    ORB_SPEC ORB_API operator glm::vec4();
#endif
#endif
  }Vector4D;

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
namespace orb
{
  // --------------------------------------------------------------------
  // 
  // System Functions
  //
  // --------------------------------------------------------------------

  /**
   * @brief Startup the ORB API.
   *
   * This should be called once and only once in a program, calling this more than once will do nothing
   */
  extern ORB_SPEC void ORB_API Initialize();
  /**
   * @brief Request a new backend API.
   *
   * This will tell ORB to initialize or change to the requested api.
   */
  extern ORB_SPEC void ORB_API RequestAPI(API_VERSION);

  extern ORB_SPEC void ORB_API EnableDebugOutput(bool b);

  /**
   * @brief Update the window
   *
   */
  extern ORB_SPEC void Update();
  extern ORB_SPEC void EnableLighting(bool);
  extern ORB_SPEC void EnableShadows(bool b);
  extern ORB_SPEC void SetMaterialProperties(Vector3D diffuse, Vector3D specular, float specular_exponent);
  extern ORB_SPEC void SetMaterial(int id);
  extern ORB_SPEC void SetLight(Vector4D pos, Vector3D color);
  /**
   * @brief Set the stored render mode. 
   * 
   * @details If stored render is enabled, render calls are deffered and are 
   * automatically executed on each shader stage that contains a vertex and fragment shader.
   * This will also ignore any passed render layer and instead render to the layer specified to the mesh. 
   * 
   */
  extern ORB_SPEC void EnableStoredRender(bool b);

  /**
   * @brief Register a function to be called during rendering.
   *
   * @param stage - the stage to call the function on.
   * @param Callback - Function pointer to callback. Callback must return non zero value if error occurs.
   */
  extern ORB_SPEC void ORB_API RegisterRenderCallback(int (*Callback)(), RENDER_STAGE stage = RENDER_STAGE::PRIMARY_RENDER, int index = 0);

  /**
   * @brief Register an event callback.
   *        Note: This will override any previously assigned callback
   *
   * @param Callback - Pointer to function to call with callback.
   */
  extern ORB_SPEC void ORB_API RegisterKeyboardCallback(KeyCallback callback);
  extern ORB_SPEC void ORB_API RegisterMouseButtonCallback(MouseButtonCallback callback);
  extern ORB_SPEC void ORB_API RegisterMouseMovementCallback(MouseMovmentCallback callback);
  extern ORB_SPEC void ORB_API RegisterWindowCallback(WindowCallback callback);
  /**
   * @brief Register a general callback for message handling. Intended for manual callback handling
   *        Note: This will override any previously assigned callback
   *        Note: This will require user to include SDL_Events to handle events
   *
   * @param Callback - Pointer to function to call with callback.
   */
  extern ORB_SPEC void ORB_API RegisterMessageCallback(GenericCallback callback);

  /**
   * @brief Tell the ORB api to shutdown.
   */
  extern ORB_SPEC void ORB_API ShutDown();
  /**
   * @brief Retrieve the current error state of thr ORB Api.
   */
  extern ORB_SPEC int  ORB_API GetError();
  /**
 * @brief Check if the window was triggered to close.
 */
  extern ORB_SPEC bool ORB_API IsRunning();

  extern ORB_SPEC Vector2D ORB_API ToScreenSpace(Vector2D);
  extern ORB_SPEC Vector2D ORB_API ToWorldSpace(Vector2D);


  // --------------------------------------------------------------------
  //
  // Window Functions
  //
  // --------------------------------------------------------------------

  /**
   * @brief Create a new Window.
   *
   * @param name - Optional window name
   *
   * @return pointer to the window data created.
   */
  extern ORB_SPEC Window* CreateNewWindow();
  extern ORB_SPEC Window* CreateNewWindow( std::string& name);


  /**
   * @brief Set the active window.
   */
  extern ORB_SPEC void SetActiveWindow(Window* w);

  /**
   * @brief Retrieve an active window.
   *  Passing no arguments or a zero will retrieve the very first window created on Initialization
   *
   * @param index - the index into the array of active windows for the window to retrieve
   *
   * @return pointer to the window data retrieved, nullptr if index is out of bounds
   */
  extern ORB_SPEC Window* RetrieveWindow(int index = 0);
  /**
   * @brief Set the clear color for a window.
   *
   * @param w - the window
   * @param color - the color
   */
  extern ORB_SPEC void ORB_API SetWindowClearColor(Window* w, Vector4D color);
  /**
   * @brief Set the position of  a window.
   * 
   * @param w - the window
   * @param x - window's new x position with 0 being the left side of the monitor
   * @param y - window's new y position with 0 being the top side of the monitor
   */
  extern ORB_SPEC void ORB_API SetWindowPosition(Window* w, int x, int y);

  /**
   * @brief Set the size of a window.
   * 
   * @param wi - the window
   * @param w - the new width
   * @param h - the new height
   */
  extern ORB_SPEC void ORB_API SetWindowScale(Window* wi, int w, int h);

  extern ORB_SPEC void ORB_API SetWindowViewport(Window* w, int vx, int vy, int vw, int vh);

  extern ORB_SPEC void ORB_API SetWindowMax(Window* w);

  extern ORB_SPEC void ORB_API SetWindowFullScreen(Window* w, int type = 0);

  // --------------------------------------------------------------------
  //
  // Render Functions
  //
  // --------------------------------------------------------------------

  /**
   * @brief Draw a 2D rectangle on-screen.
   *
   * @param x - xposition
   * @param y - yposition
   * @param width - rectangle's width
   * @param height - rectangle's height
   */
  extern ORB_SPEC void ORB_API DrawRect(float x, float y, float width, float height, int layer = 1);
  extern ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale, int layer = 1);

  /**
   * @brief Draw a 2D rectangle on-screen.
   *
   * @param x - xposition
   * @param y - yposition
   * @param width - rectangle's width
   * @param height - rectangle's height
   * @param rotation - rectangle's 2D rotation
   */
  extern ORB_SPEC void ORB_API DrawRectAdvanced(float x, float y, float width, float height, float rotation, int layer = 1);
  extern ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation, int layer = 1);
  /**
   * @brief Set the active color being drawn. Defaults to full opacity
   * @param r - Red component
   * @param g - Green component
   * @param b - Blue component
   * @param a - Alpha component
   */
  extern ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a = 255);
  /**
 * @brief Draws a line in 2D or 3D space.
 *
 * This function draws a line from the specified start point to the specified end point in either
 * 2D or 3D space. The depth parameter determines the rendering order, with lower values rendering
 * behind higher values.
 *
 * @param start -  The starting point of the line. Use a Vector2D for 2D space or Vector3D for 3D space.
 * @param end   - The ending point of the line. Use a Vector2D for 2D space or Vector3D for 3D space.
 * @param depth - The depth or layer on which the line will be rendered (default is 1).
 *
 * @note The depth parameter is used to control the rendering order, with lower values rendering behind higher values.
 */
  extern ORB_SPEC void ORB_API DrawLine(Vector2D start, Vector2D end, int depth = 1);
  extern ORB_SPEC void ORB_API DrawLine(Vector3D start, Vector3D end, int depth = 1);
  /**
   * @brief Set the project mode to use, default behavior is orthogonal projection.
   *
   * @param p - Projection typ enum
   *
   */
  extern ORB_SPEC void ORB_API SetProjectionMode(PROJECTION_TYPE);
  /**
   * @brief Set the Drawmode of the Mesh. (Default = 6)
   *
   * @param mode - The mode to draw the mesh with:
   * 0 = Points
   * 1 = Lines
   * 2 = Line Loop
   * 3 = Line Strip
   * 4 = Triangles
   * 5 = Triangle Strip
   * 6 = Triangle Fan
   */
  extern ORB_SPEC void ORB_API SetDefaultRenderMode(int i);
  /**
   * @brief Set the fill mode.
   *
   * @param f - the new fill mode
   * 0 = Point
   * 1 = Lines
   * 2 = Fill
   */
  extern ORB_SPEC void ORB_API SetFillMode(int f);
  /**
   * @brief Set the zoom level.
   *
   * @param z - the new zoom
   */
  extern ORB_SPEC void ORB_API SetZoom(float z);
  /**
   * @brief Get the current zoom.
   */
  extern ORB_SPEC float ORB_API GetZoom();
  /**
   * @brief Get the size of the window passed in.
   */
  extern ORB_SPEC Vector2D ORB_API GetWindowSize(Window*);
  /**
   * @brief Get the position of the camera.
   */
  extern ORB_SPEC Vector3D ORB_API GetCameraPosition();
  /**
   * @brief Set the camera's position.
   *
   * @param pos - the camera's new position
   */
  extern ORB_SPEC void ORB_API SetCameraPosition(Vector3D pos);
  /**
   * @brief Set the camera's rotation.
   *
   * @param rot - the camera's new 3D rotation
   */
  extern ORB_SPEC void ORB_API SetCameraRotation(Vector3D rot);
  extern ORB_SPEC void ORB_API SetCameraRotation(Vector4D rot);

  // --------------------------------------------------------------------
  //
  // Texture Functions
  //
  // --------------------------------------------------------------------

  /**
   * @brief Load a texture from a file.
   *
   * @param path - path to the file
   * @return Returns a pointer to the Texture data structure used in ORB to manage texture
   */
  extern ORB_SPEC ORB_texture ORB_API LoadTexture( std::string& path);
  /**
 * @brief Load a texture from a file.
 *
 * @param path - path to the file
 * @return Returns a pointer to the Texture data structure used in ORB to manage texture
 */
  extern ORB_SPEC ORB_texture ORB_API LoadTexture(const char* path);
  /**
   * @brief Get a constant vector holding pointers to all the currently loaded textures.
   */
  extern ORB_SPEC std::vector<ORB_texture> const& ORB_API GetAllLoadedTextures();
  /**
   * @brief Set the active Texture being renderer, passing a null pointer will remove the current texture.
   */
  extern ORB_SPEC void ORB_API SetActiveTexture(ORB_texture);
  /**
   * @brief Delete a texture.
   */
  extern ORB_SPEC void ORB_API DeleteTexture(ORB_texture);
  /**
   * @brief Get the width and height of a Texture.
   */
  extern ORB_SPEC Vector2D ORB_API GetTextureDimension(ORB_texture);
  /**
   * @brief Sets the texture coordinates (UV) using a Vector2D.
   *
   * This function sets the texture coordinates (UV) using the provided Vector2D.
   *
   * @param uv The sub-texture's center UV coordinates as a Vector2D.
   * @param scale The scale of the sub-texuture as a Vector2D
   */
  extern ORB_SPEC void ORB_API SetUV(Vector2D const& center, Vector2D const& scale);
  /**
   * @brief Sets the texture coordinates (UV) using float values.
   *
   * This function sets the texture coordinates (UV) using the provided float values for u and v.
   *
   * @param u The center u-coordinate of the sub-texture.
   * @param v The center v-coordinate of the sub-texture.
   * @param w The sub-texture's width
   * @param h The sub-texture's height
   */
  extern ORB_SPEC void ORB_API SetUV(float u, float v, float w, float h);
#ifdef ORB_GLM

  /**
   * @brief Sets the texture coordinates (UV) using a 4x4 matrix.
   *
   * This function sets the texture coordinates (UV) using the specified 4x4 matrix.
   *
   * @param matrix The 4x4 matrix containing the texture coordinates.
   */
  extern ORB_SPEC void ORB_API SetUV(glm::mat4 const& uv);
#endif

  extern ORB_SPEC void ORB_API SetTextureSampleMode(ORB_texture t, SAMPLE_SCALE_MODE ssm);


  // --------------------------------------------------------------------
  //
  // Mesh Functions
  //
  // --------------------------------------------------------------------

  /**
   * @brief Start a new default Mesh.
   */
  extern ORB_SPEC void ORB_API BeginMesh();
  /**
   * @brief Start a new Textured Mesh.
   */
  extern ORB_SPEC void ORB_API BeginTexMesh();
  /**
   * @brief Set the Drawmode of the Mesh. (Default = 6)
   *
   * @param mode - The mode to draw the mesh with:
   * 0 = Points
   * 1 = Lines
   * 2 = Line Loop
   * 3 = Line Strip
   * 4 = Triangles
   * 5 = Triangle Strip
   * 6 = Triangle Fan
   */
  extern ORB_SPEC void ORB_API MeshSetDrawMode(int mode);
  /**
   * @brief Add a vertex to the active mesh.
   * Must be called after BeginMesh()
   *
   * @param pos - The position in mesh space 2D or 3D
   * @param color - Either a 3 component RGB or a 4 component RGBA color
   * @param UV - The vertice's UV coordinates
   */
  extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos);
  extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color);
  extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector3D color);
  extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV);
  extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV, Vector4D norm);

  /**
   * @brief Set the draw color of the active Mesh.
   */
  extern ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color);
  /**
   * @brief Set the texture of the active TextureMesh.
   *  Note: Calling this while the active Mesh is not a Texture Mesh will do nothing
   */
  extern ORB_SPEC void ORB_API TexMeshSetTexture(ORB_texture t);
  extern ORB_SPEC void ORB_API TexMeshSetTexture( std::string& s);
  extern ORB_SPEC void ORB_API TexMeshSetTexture(const char* s);

  /**
   * @brief End the mesh creation and return handle to internal mesh.
   */
  extern ORB_SPEC ORB_mesh ORB_API EndMesh();
  /**
   * @brief Create a mesh from a file path.
   *
   * @param path - path to mesh to load
   */
  extern ORB_SPEC ORB_mesh ORB_API LoadMesh(const char*);
  extern ORB_SPEC ORB_mesh ORB_API LoadMesh( std::string& s);

  /**
   * @brief Create a textured mesh from a file path.
   *
   * @param path - path to teh mesh to load
   */
  extern ORB_SPEC ORB_mesh ORB_API LoadTexMesh(const char* c);
  extern ORB_SPEC ORB_mesh ORB_API LoadTexMesh( std::string& s);
  /**
   * @brief Draw a mesh object.
   *
   * @param m - the mesh to draw
   * @param pos - the **world** position to draw at
   * @param scale - the objects scale
   * @param rot - the objects 3D rotation in radians along each axis
   */
  extern ORB_SPEC void ORB_API DrawMesh(ORB_mesh m, Vector3D const& pos, Vector3D const& scale, Vector3D const& rot, int layer = 1);
#ifdef ORB_GLM
  extern ORB_SPEC void ORB_API DrawMesh(ORB_mesh m, glm::mat4 matrix, int layer = 1);
#endif
  extern ORB_SPEC void ORB_API MeshSetLayer(ORB_mesh m, int l);

  // --------------------------------------------------------------------
  //
  // Text and Font Functions
  //
  // --------------------------------------------------------------------
  /**
   * @brief Load a font for use.
   *
   * @param path - Path to the font file to load
   *
   * @return abstract pointer to FontInfo struct used in backend, nullptr if load failed.
   */
  extern ORB_SPEC ORB_font ORB_API LoadFont(const char* path);
  /**
   * @brief Unload a font.
   *
   * @param font - the font to unload.
   */
  extern ORB_SPEC void ORB_API DestroyFont(ORB_font font);
  /**
   * @brief Set the font to be active for draw.
   *
   * @param font - The font to set active
   */
  extern ORB_SPEC void ORB_API SetActiveFont(ORB_font f);
  /**
   * @brief Renders the specified text to a texture.
   *
   * This function takes a text string, font size, and color as input and generates a texture
   * containing the rendered text. The texture is then returned.
   *
   * @param text The text to be rendered.
   * @param size The font size of the text.
   * @param color The color of the text as a Vector4D (default is {1, 1, 1, 1}).
   * @return The texture containing the rendered text.
   *
   * @note Make sure to release the returned texture when it is no longer needed to avoid memory leaks.
   */
  extern ORB_SPEC ORB_texture ORB_API RenderTextToTexture(const char* text, int size, Vector4D const& color = { 1,1,1,1 });
  /**
   * @brief Writes text to the screen at the specified position.
   *
   * This function writes the specified text to the screen at the given position, with the
   * specified font size, color, and layer.
   *
   * @param text The text to be written.
   * @param pos The position on the screen where the text will be written.
   * @param size The font size of the text.
   * @param color The color of the text as a Vector4D (default is {1, 1, 1, 1}).
   * @param layer The layer on which the text will be rendered (default is 1, Max is 2).
   *
   * @note The layer parameter determines the rendering order, with lower values rendering behind higher values.
   */
  extern ORB_SPEC void ORB_API WriteText(const char* text, Vector2D const& pos, int size, Vector4D const& color = { 1,1,1,1 }, int layer = 1);

  // --------------------------------------------------------------------
  //
  // Advanced Render Functions
  //
  // --------------------------------------------------------------------
  /**
  // The ORB API makes use of a `Render Pass` / `Shader Stage` model for rendering
  // Each program currently can have 1 active `Render Pass`, but each `Render Pass` may have
  // any number of `Shader Stages`.
  // Shader Stages are organized into different `Render Stages`.
  // The 6 `Render Stages` are
  //   - `Pre Render`
  //   - `Primary Render`
  //   - `Secondary Render`
  //   - `Post Render`
  //   - `Pre-Frame Swap`
  //   - `Post-Frame Swap`
  // Each stage gets executed in order every frame.
  // For a `Render Stage` to be executed it must have at least 1 associated `Shader Stage`.
  // A `Shader Stage` must have at least 1 `Callback function` associated with it for it to be run. The callback
  // can be associated with an `id` and a `Render Stage`. The `id` is used to determine which `Shader Stage` to
  // call the function during.
  //      EX: A `Render Stage` has 3 `Shader Stages` associated with it. Function "Foo" has `id` 1,
  //          Function "Bar" has `id` 2, Function "Baz" has `id` 1, Function "zip" has `id` 3.
  //          The functions will be called in the order: Foo, Baz, Bar, Zip.
  //
  // A `Shader Stage` has an associated `.meta` file that contains information about the stage's content and files
  //
  // A `Render Pass` has is denoted by a `.rpass.meta` file. This file contains information about all the
  // shader stages and frame buffers within it
  */
  // --------------------------------------------------------------------

  /**
   * @brief Load a custom Render Pass (.rpass.meta) from file.
   *        Note: This will replace the currently active Render Pass, custom or default.
   *
   * @param path - Path to the .rpass.meta file to load from
   */
  extern ORB_SPEC void ORB_API LoadCustomRenderPass(std::string const& path);
  extern ORB_SPEC void ORB_API LoadCustomRenderPass(const char* path);


  extern ORB_SPEC void ORB_API SetBufferBase( std::string& buffer, int base);
  extern ORB_SPEC void ORB_API SetBufferBase(const char*, int base);


  /**
   * @brief Write data to a buffer.
   *        Note: This function binds and un binds the buffer being written to
   * @param buffer the buffer name to write to
   * @param dataSize the size of the data
   * @param data the data to write
   */
  extern ORB_SPEC void ORB_API WriteBuffer( std::string& buffer, size_t dataSize, void* data);
  extern ORB_SPEC void ORB_API WriteBuffer(const char* buffer, size_t dataSize, void* data);

  /**
   * @brief Write an uniform.
   *
   * @param buffer the attribute name to write
   * @param data the data
   */
  extern ORB_SPEC void ORB_API WriteUniform( std::string& buffer, void* data);
  extern ORB_SPEC void ORB_API WriteUniform(const char* buffer, void* data);

  /**
   * @brief Dispatch a compute shader.
   *        Note: If the currently active Shader stage is not a compute shader
   *              then this does nothing
   *
   * @param x - Workgroup count in x
   * @param y - Workgroup count in y
   * @param z - Workgroup count in z
   */
  extern ORB_SPEC void ORB_API DispatchCompute(int x, int y, int z);

  /**
   * @brief Write to a specific index in a buffer.
   *        Note: This function binds and un binds the buffer being written to
   * @param buffer the buffer name to write to
   * @param index the index into the data
   * @param structSize the size of one single element
   * @param data the data to write
   */
  extern  ORB_SPEC void ORB_API WriteSubBufferData( std::string& buffer, int index, size_t structSize, void* data);
  extern  ORB_SPEC void ORB_API WriteSubBufferData(const char* buffer, int index, size_t structSize, void* data);

  /**
   * @brief Update the render constants in the current shader stage
   */
  extern ORB_SPEC void ORB_API WriteRenderConstantsHere();

  /**
   * @brief Bind a texture to a specific texture unit.
   * 
   * @param tex - opaque texure pointer
   * @param unit - texture unit to bind to
   */
  extern ORB_SPEC void ORB_API SetBindTextureToUnit(ORB_texture tex, int unit);
  
  /**
   * @brief Draw a mesh multiple times in one draw call.
   * 
   * @param m the mesh 
   * @param count how many instances to draw
   */
  extern ORB_SPEC void ORB_API DrawIndexed(ORB_mesh m, int count);
  /**
   * @brief Get an FBO object by name.
   *
   * This function retrieves an FBO object by its name.
   *
   * @param name The name of the FBO.
   * @return ORB_FBO The FBO object.
   */
  extern ORB_SPEC ORB_FBO ORB_API GetFBOByName(const char* name);
  extern ORB_SPEC ORB_FBO ORB_API GetFBOByName( std::string& name);
  /**
   * @brief Bind an FBO to the OpenGL context.
   *
   * This function binds the specified FBO to the active OpenGL context.
   *
   * @param fbo The FBO to be bound.
   */
  extern ORB_SPEC void ORB_API BindActiveFBO(ORB_FBO);
  
  extern ORB_SPEC void ORB_API ClearFBO(ORB_FBO);
  /**
   * @brief Set an FBO texture as active.
   *
   * This function sets the specified FBO texture as active for rendering.
   *
   * @param fbo The FBO whose texture is to be set active.
   * @param binding The texture binding index. Default is 0.
   */
  extern ORB_SPEC void ORB_API SetFBOTextureActive(ORB_FBO, int binding = 0);

  extern ORB_SPEC void ORB_API SetBlendMode(BLEND_MODE);
  extern ORB_SPEC void ORB_API DumpMesh(ORB_mesh m);




}
#endif
#ifdef __cplusplus
extern "C" {
#endif
  /**
 * @brief Startup the ORB API.
 *
 * This should be called once and only once in a program, calling this more than once will do nothing
 */
extern ORB_SPEC void ORB_API Initialize();
/**
 * @brief Request a new backend API.
 *
 * This will tell ORB to initialize or change to the requested api.
 */
extern ORB_SPEC void ORB_API RequestAPI(API_VERSION);

extern ORB_SPEC void ORB_API EnableDebugOutput(bool b);

/**
 * @brief Update the window
 *
 */
extern ORB_SPEC void Update();
extern ORB_SPEC void EnableLighting(bool b);
extern ORB_SPEC void EnableShadows(bool b);
extern ORB_SPEC void SetMaterialProperties(Vector3D diffuse, Vector3D specular, float specular_exponent);
extern ORB_SPEC void SetMaterial(int id);

extern ORB_SPEC void SetLight(Vector4D pos, Vector3D color);

extern ORB_SPEC void EnableStoredRender(bool b);
/**
 * @brief Register a function to be called during rendering.
 *
 * @param stage - the stage to call the function on.
 * @param Callback - Function pointer to callback. Callback must return non zero value if error occurs.
 */
extern ORB_SPEC void ORB_API RegisterRenderCallback(int (*Callback)(), RENDER_STAGE stage, int index);

/**
 * @brief Register an event callback.
 *        Note: This will override any previously assigned callback
 *
 * @param Callback - Pointer to function to call with callback.
 */
extern ORB_SPEC void ORB_API RegisterKeyboardCallback(KeyCallback callback);
extern ORB_SPEC void ORB_API RegisterMouseButtonCallback(MouseButtonCallback callback);
extern ORB_SPEC void ORB_API RegisterMouseMovementCallback(MouseMovmentCallback callback);
extern ORB_SPEC void ORB_API RegisterWindowCallback(WindowCallback callback);
/**
 * @brief Register a general callback for message handling. Intended for manual callback handling
 *        Note: This will override any previously assigned callback
 *        Note: This will require user to include SDL_Events to handle events
 *
 * @param Callback - Pointer to function to call with callback.
 */
extern ORB_SPEC void ORB_API RegisterMessageCallback(GenericCallback callback);

/**
 * @brief Tell the ORB api to shutdown.
 */
extern ORB_SPEC void ORB_API ShutDown();
/**
 * @brief Retrieve the current error state of thr ORB Api.
 */
extern ORB_SPEC int  ORB_API GetError();
/**
* @brief Check if the window was triggered to close.
*/
extern ORB_SPEC int ORB_API IsRunning();

extern ORB_SPEC Vector2D ORB_API ToScreenSpace(Vector2D);
extern ORB_SPEC Vector2D ORB_API ToWorldSpace(Vector2D);

extern ORB_SPEC Window* CreateNewWindow(const char* c);
extern ORB_SPEC SDL_Window* GetWindowHandle(Window*);
extern ORB_SPEC SDL_GLContext GetGLContext(Window*);
extern ORB_SPEC SDL_GLContext GetCurrentGLContext();

/**
 * @brief Set the active window.
 */
extern ORB_SPEC void ORBActiveWindow(Window* w);

/**
 * @brief Retrieve an active window.
 *  Passing no arguments or a zero will retrieve the very first window created on Initialization
 *
 * @param index - the index into the array of active windows for the window to retrieve
 *
 * @return pointer to the window data retrieved, nullptr if index is out of bounds
 */
extern ORB_SPEC Window* RetrieveWindow(int index);
/**
 * @brief Set the clear color for a window.
 *
 * @param w - the window
 * @param color - the color
 */
extern ORB_SPEC void ORB_API SetWindowClearColor(Window* w, Vector4D color);
/**
 * @brief Set the position of  a window.
 *
 * @param w - the window
 * @param x - window's new x position with 0 being the left side of the monitor
 * @param y - window's new y position with 0 being the top side of the monitor
 */
extern ORB_SPEC void ORB_API SetWindowPosition(Window* w, int x, int y);

/**
 * @brief Set the size of a window.
 *
 * @param wi - the window
 * @param w - the new width
 * @param h - the new height
 */
extern ORB_SPEC void ORB_API SetWindowScale(Window* wi, int w, int h);

extern ORB_SPEC void ORB_API SetWindowViewport(Window* w, int vx, int vy, int vw, int vh);

extern ORB_SPEC void ORB_API SetWindowMax(Window* w);

extern ORB_SPEC void ORB_API SetWindowFullScreen(Window* w, int type);

/**
 * @brief Draw a 2D rectangle on-screen.
 *
 * @param x - xposition
 * @param y - yposition
 * @param width - rectangle's width
 * @param height - rectangle's height
 */
extern ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale, int layer);

/**
 * @brief Draw a 2D rectangle on-screen.
 *
 * @param x - xposition
 * @param y - yposition
 * @param width - rectangle's width
 * @param height - rectangle's height
 * @param rotation - rectangle's 2D rotation
 */
extern ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation, int layer);
/**
 * @brief Set the active color being drawn. Defaults to full opacity
 * @param r - Red component
 * @param g - Green component
 * @param b - Blue component
 * @param a - Alpha component
 */
extern ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a);
/**
* @brief Draws a line in 2D or 3D space.
*
* This function draws a line from the specified start point to the specified end point in either
* 2D or 3D space. The depth parameter determines the rendering order, with lower values rendering
* behind higher values.
*
* @param start -  The starting point of the line. Use a Vector2D for 2D space or Vector3D for 3D space.
* @param end   - The ending point of the line. Use a Vector2D for 2D space or Vector3D for 3D space.
* @param depth - The depth or layer on which the line will be rendered (default is 1).
*
* @note The depth parameter is used to control the rendering order, with lower values rendering behind higher values.
*/
extern ORB_SPEC void ORB_API DrawLine(Vector3D start, Vector3D end, int depth);
/**
 * @brief Set the project mode to use, default behavior is orthogonal projection.
 *
 * @param p - Projection typ enum
 *
 */
extern ORB_SPEC void ORB_API SetProjectionMode(PROJECTION_TYPE);
/**
 * @brief Set the Drawmode of the Mesh. (Default = 6)
 *
 * @param mode - The mode to draw the mesh with:
 * 0 = Points
 * 1 = Lines
 * 2 = Line Loop
 * 3 = Line Strip
 * 4 = Triangles
 * 5 = Triangle Strip
 * 6 = Triangle Fan
 */
extern ORB_SPEC void ORB_API SetDefaultRenderMode(int i);
/**
 * @brief Set the fill mode.
 *
 * @param f - the new fill mode
 * 0 = Point
 * 1 = Lines
 * 2 = Fill
 */
extern ORB_SPEC void ORB_API SetFillMode(int f);
/**
 * @brief Set the zoom level.
 *
 * @param z - the new zoom
 */
extern ORB_SPEC void ORB_API SetZoom(float z);
/**
 * @brief Get the current zoom.
 */
extern ORB_SPEC float ORB_API GetZoom();
/**
 * @brief Get the size of the window passed in.
 */
extern ORB_SPEC float ORB_API GetWindowSizeX(Window*);
extern ORB_SPEC float ORB_API GetWindowSizeY(Window*);
/**
 * @brief Get the position of the camera.
 */
extern ORB_SPEC Vector3D ORB_API GetCameraPosition();
/**
 * @brief Set the camera's position.
 *
 * @param pos - the camera's new position
 */
extern ORB_SPEC void ORB_API SetCameraPosition(Vector3D pos);
/**
 * @brief Set the camera's rotation.
 *
 * @param rot - the camera's new 3D rotation
 */
extern ORB_SPEC void ORB_API SetCameraRotationAngles(Vector3D rot);
extern ORB_SPEC void ORB_API SetCameraRotation(Vector4D rot);
// --------------------------------------------------------------------
//
// Texture Functions
//
// --------------------------------------------------------------------

/**
* @brief Load a texture from a file.
*
* @param path - path to the file
* @return Returns a pointer to the Texture data structure used in ORB to manage texture
*/
extern ORB_SPEC ORB_texture ORB_API LoadTexture(const char* path);
/**
 * @brief Set the active Texture being renderer, passing a null pointer will remove the current texture.
 */
extern ORB_SPEC void ORB_API SetActiveTexture(ORB_texture);
/**
 * @brief Delete a texture.
 */
extern ORB_SPEC void ORB_API DeleteTexture(ORB_texture);
/**
 * @brief Get the width and height of a Texture.
 */
extern ORB_SPEC Vector2D ORB_API GetTextureDimension(ORB_texture);
/**
 * @brief Sets the texture coordinates (UV) using a Vector2D.
 *
 * This function sets the texture coordinates (UV) using the provided Vector2D.
 *
 * @param uv The sub-texture's center UV coordinates as a Vector2D.
 * @param scale The scale of the sub-texuture as a Vector2D
 */
extern ORB_SPEC void ORB_API SetUV(Vector2D const* center, Vector2D const* scale);



extern ORB_SPEC void ORB_API SetTextureSampleMode(ORB_texture t, enum SAMPLE_SCALE_MODE ssm);


// -------
// Mesh Functions
//
// --------------------------------------------------------------------

/**
 * @brief Start a n-------------------------------------------------------------
//ew default Mesh.
 */
extern ORB_SPEC void ORB_API BeginMesh();
/**
 * @brief Start a new Textured Mesh.
 */
extern ORB_SPEC void ORB_API BeginTexMesh();
/**
 * @brief Set the Drawmode of the Mesh. (Default = 6)
 *
 * @param mode - The mode to draw the mesh with:
 * 0 = Points
 * 1 = Lines
 * 2 = Line Loop
 * 3 = Line Strip
 * 4 = Triangles
 * 5 = Triangle Strip
 * 6 = Triangle Fan
 */
extern ORB_SPEC void ORB_API MeshSetDrawMode(int mode);
/**
 * @brief Add a vertex to the active mesh.
 * Must be called after BeginMesh()
 *
 * @param pos - The position in mesh space 2D or 3D
 * @param color - Either a 3 component RGB or a 4 component RGBA color
 * @param UV - The vertice's UV coordinates
 */
extern ORB_SPEC void ORB_API MeshAddVertex(Vector3D pos, Vector4D color, Vector2D UV);
/**
 * @brief Set the draw color of the active Mesh.
 */
extern ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color);
/**
 * @brief Set the texture of the active TextureMesh.
 *  Note: Calling this while the active Mesh is not a Texture Mesh will do nothing
 */
extern ORB_SPEC void ORB_API TexMeshSetTextureFromPointer(ORB_texture t);
extern ORB_SPEC void ORB_API TexMeshSetTextureFromString(const char* s);

/**
 * @brief End the mesh creation and return handle to internal mesh.
 */
extern ORB_SPEC ORB_mesh ORB_API EndMesh();
/**
 * @brief Create a mesh from a file path.
 *
 * @param path - path to mesh to load
 */
extern ORB_SPEC ORB_mesh ORB_API LoadMesh(const char*);

/**
 * @brief Create a textured mesh from a file path.
 *
 * @param path - path to teh mesh to load
 */
extern ORB_SPEC ORB_mesh ORB_API LoadTexMesh(const char* c);
/**
 * @brief Draw a mesh object.
 *
 * @param m - the mesh to draw
 * @param pos - the **world** position to draw at
 * @param scale - the objects scale
 * @param rot - the objects 3D rotation in radians along each axis
 */
extern ORB_SPEC void ORB_API DrawMesh(ORB_mesh m, Vector3D const* pos, Vector3D const* scale, Vector3D const* rot, int layer);
extern ORB_SPEC void ORB_API DrawMeshMatrix(ORB_mesh m, float mat[4][4], int layer);
extern ORB_SPEC void ORB_API MeshSetLayer(ORB_mesh m, int l);

// --------------------------------------------------------------------
//
// Text and Font Functions
//
// --------------------------------------------------------------------
/**
 * @brief Load a font for use.
 *
 * @param path - Path to the font file to load
 *
 * @return abstract pointer to FontInfo struct used in backend, nullptr if load failed.
 */
extern ORB_SPEC ORB_font ORB_API LoadFont(const char* path);
/**
 * @brief Unload a font.
 *
 * @param font - the font to unload.
 */
extern ORB_SPEC void ORB_API DestroyFont(ORB_font font);
/**
 * @brief Set the font to be active for draw.
 *
 * @param font - The font to set active
 */
extern ORB_SPEC void ORB_API SetActiveFont(ORB_font f);
/**
 * @brief Renders the specified text to a texture.
 *
 * This function takes a text string, font size, and color as input and generates a texture
 * containing the rendered text. The texture is then returned.
 *
 * @param text The text to be rendered.
 * @param size The font size of the text.
 * @param color The color of the text as a Vector4D (default is {1, 1, 1, 1}).
 * @return The texture containing the rendered text.
 *
 * @note Make sure to release the returned texture when it is no longer needed to avoid memory leaks.
 */
extern ORB_SPEC ORB_texture ORB_API RenderTextToTexture(const char* text, int size, Vector4D const* color);
/**
 * @brief Writes text to the screen at the specified position.
 *
 * This function writes the specified text to the screen at the given position, with the
 * specified font size, color, and layer.
 *
 * @param text The text to be written.
 * @param pos The position on the screen where the text will be written.
 * @param size The font size of the text.
 * @param color The color of the text as a Vector4D (default is {1, 1, 1, 1}).
 * @param layer The layer on which the text will be rendered (default is 1, Max is 2).
 *
 * @note The layer parameter determines the rendering order, with lower values rendering behind higher values.
 */
extern ORB_SPEC void ORB_API WriteText(const char* text, Vector2D const* pos, int size, Vector4D const* color, int layer);

extern ORB_SPEC void ORB_API DumpMesh(ORB_mesh);

extern ORB_SPEC void ORB_API MeshSetUIMode(ORB_mesh, bool);

#ifdef __cplusplus
}
#endif