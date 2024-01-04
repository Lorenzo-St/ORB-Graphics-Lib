/*********************************************************************
 * @file   OverloadedRenderBackend.h
 * @brief
 *
 * @author Lorenzo St. Luce
 * @date   December 2023
 *********************************************************************/
#pragma once

#ifdef __cplusplus
#define ORB_ENUM enum class
#else
#define ORB_ENUM enum
#endif
#define ORB_API __cdecl

#ifdef ORB_BUILD
#define ORB_SPEC __declspec(dllexport)
#else
#define ORB_SPEC
#endif


typedef ORB_ENUM API_VERSION
{
  OPENGL_LATEST,
  OPENGL_450,
  OPENGL_430,
  VULKAN,
  DIRECTX
}API_VERSION;



typedef ORB_ENUM RENDER_STAGE
{
  PRERENDER,
  PRIMARY_RENDER,
  SECONDARY_RENDER,
  POST_RENDER,
  PRE_FRAME_SWAP,
  POST_FRAME_SWAP
}RENDER_STAGE;

typedef ORB_ENUM PROJECTION_TYPE
{
  ORTHOGONAL,
  PERSPECTIVE
}PROJECTION_TYPE;

typedef ORB_ENUM KEY_STATE
{
  RELEASED = 0,
  PRESSED = 1,
  HELD = 2,
}KEY_STATE;


typedef struct Window Window;

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct Texture Texture;
typedef Texture* texture;

typedef struct Mesh Mesh;
typedef Mesh const* mesh;

typedef struct FontInfo FontInfo;
typedef FontInfo const* font;

typedef union SDL_Event SDL_Event;
typedef SDL_Event* Event;

typedef void(*KeyCallback)(char key, KEY_STATE state);
typedef void(*MouseButtonCallback)(int button, KEY_STATE state);
typedef void(*MouseMovmentCallback)(int x, int y, int deltaX, int deltaY);
typedef void (*GenericCallback)(Event);

typedef struct Vector4D Vector4D;
typedef struct Vector3D Vector3D;


typedef struct Vector2D
{
  float x;
  float y;
#ifdef __cplusplus
  Vector2D() = default;
  Vector2D(float _x, float _y);
  Vector2D(Vector3D const& a);
  Vector2D(Vector4D const& a);
#endif

}Vector2D;
typedef struct Vector3D
{
  float x;
  float y;
  float z;
#ifdef __cplusplus
  Vector3D() = default;
  Vector3D(float _x, float _y, float _z);
  Vector3D(float _x, float _y);

  Vector3D(Vector2D const& a);
  Vector3D(Vector4D const& a);
#endif
}Vector3D;
typedef struct Vector4D
{
  float r;
  float g;
  float b;
  float a;
#ifdef __cplusplus
  Vector4D() = default;
  Vector4D(float _r, float _g, float _b, float _a);
  Vector4D(Vector2D const& a);
  Vector4D(Vector3D const& a);
#endif
}Vector4D;


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
  /**
   * @brief Update the window
   *
   */
  extern ORB_SPEC void Update();

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
  extern ORB_SPEC Window* CreateNewWindow(std::string name);


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

  extern ORB_SPEC void ORB_API SetWindowClearColor(Window* w, Vector4D color);

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
  extern ORB_SPEC void ORB_API DrawRect(float x, float y, float width, float height);
  extern ORB_SPEC void ORB_API DrawRect(Vector2D pos, Vector2D scale);

  /**
   * @brief Draw a 2D rectangle on-screen.
   *
   * @param x - xposition
   * @param y - yposition
   * @param width - rectangle's width
   * @param height - rectangle's height
   * @param rotation - rectangle's 2D rotation
   */
  extern ORB_SPEC void ORB_API DrawRectAdvanced(float x, float y, float width, float height, float rotation);
  extern ORB_SPEC void ORB_API DrawRectAdvanced(Vector2D pos, Vector2D scale, float rotation);


  /**
   * @brief Set the active color being drawn. Defaults to full opacity
   * @param r - Red component
   * @param g - Green component
   * @param b - Blue component
   * @param a - Alpha component
   */
  extern ORB_SPEC void ORB_API SetDrawColor(uchar r, uchar g, uchar b, uchar a = 255);
  /**
   * @brief Set the project mode to use, default behavior is orthogonal projection.
   *
   * @param p - Projection typ enum
   *
   */
  extern ORB_SPEC void ORB_API SetProjectionMode(PROJECTION_TYPE);

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
  extern ORB_SPEC texture ORB_API LoadTexture(std::string path);
  /**
 * @brief Load a texture from a file.
 *
 * @param path - path to the file
 * @return Returns a pointer to the Texture data structure used in ORB to manage texture
 */
  extern ORB_SPEC texture ORB_API LoadTexture(const char* path);

  /**
   * @brief Set the active Texture being renderer, passing a null pointer will remove the current texture.
   */
  extern ORB_SPEC void ORB_API SetActiveTexture(texture);

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
  /**
   * @brief Set the draw color of the active Mesh.
   */
  extern ORB_SPEC void ORB_API MeshSetDrawColor(Vector4D color);
  /**
   * @brief Set the texture of the active TextureMesh.
   *  Note: Calling this while the active Mesh is not a Texture Mesh will do nothing
   */
  extern ORB_SPEC void ORB_API TexMeshSetTexture(texture t);
  extern ORB_SPEC void ORB_API TexMeshSetTexture(std::string s);
  extern ORB_SPEC void ORB_API TexMeshSetTexture(const char* s);

  /**
   * @brief End the mesh creation and return handle to internal mesh.
   */
  extern ORB_SPEC mesh ORB_API EndMesh();
  /**
   * @brief Create a mesh from a file path.
   *
   * @param path - path to mesh to load
   */
  extern ORB_SPEC mesh ORB_API LoadMesh(const char*);
  extern ORB_SPEC mesh ORB_API LoadMesh(std::string s);

  /**
   * @brief Create a textured mesh from a file path.
   *
   * @param path - path to teh mesh to load
   */
  extern ORB_SPEC mesh ORB_API LoadTexMesh(const char* c);
  extern ORB_SPEC mesh ORB_API LoadTexMesh(std::string s);
  /**
   * @brief Draw a mesh object.
   * 
   * @param m - the mesh to draw
   * @param pos - the **world** position to draw at
   * @param scale - the objects scale
   * @param rot - the objects 3D rotation in radians along each axis
   */
  extern ORB_SPEC void ORB_API DrawMesh(mesh m, Vector3D const& pos, Vector3D const& scale, Vector3D const& rot);

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
  extern ORB_SPEC font ORB_API LoadFont(const char* path);
  /**
   * @brief Unload a font.
   * 
   * @param font - the font to unload.
   */
  extern ORB_SPEC void ORB_API DestroyFont(font font);
  /**
   * @brief Set the font to be active for draw.
   * 
   * @param font - The font to set active
   */
  extern ORB_SPEC void ORB_API SetActiveFont(font f);

  extern ORB_SPEC texture ORB_API RenderTextToTexture(const char* text, int size, Vector4D const& color = {1,1,1,1});

  extern ORB_SPEC void ORB_API WriteText(const char* text, Vector2D const& pos, int size, Vector4D const& color = {1,1,1,1});

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
  extern ORB_SPEC void ORB_API LoadCustomRenderPass(std::string path);
  /**
   * @brief Write data to a buffer.
   *        Note: This function binds and un binds the buffer being written to
   * @param buffer the buffer name to write to
   * @param dataSize the size of the data
   * @param data the data to write
   */
  extern ORB_SPEC void ORB_API WriteBuffer(std::string buffer, size_t dataSize, void* data);
  /**
   * @brief Write an uniform.
   *
   * @param buffer the attribute name to write
   * @param data the data
   */
  extern ORB_SPEC void ORB_API WriteUniform(std::string buffer, void* data);
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
}



#endif
