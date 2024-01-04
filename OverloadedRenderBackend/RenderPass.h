/*********************************************************************
 * @file   RenderPass.h
 * @brief  Renderpass class definition
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   October 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once
#include <glad.h>
#include <unordered_map>
#include <map>
#include <array>
class ShaderStage;
// RenderPass
// ----------------------------------
// ----------------------------------
// Can have multiple shader stages
//
// Can declare certain stages to different stages of the render update
//
// All stages must have a runtime order ID, which says in what order it will be run during each
// stage's update IE: in the primary Render set, shaders in this stage will be run from ID 0 to n.

// These define where in the RenderPass Update will the shader stage be called
enum class renderStage : int
{
    PreRender,
    PrimaryRender,
    SecondaryRender,
    PostRender,
    PreFrameSwap,
    PostFrameSwap,
    End
};

/**@typedef
 * @brief A storage object for keeping track of all loaded shader stages
 *
 * @details renderStage - The stage that this shader pass will run in
 *          unsigned int - The runtime order Id of the ShaderStage
 *          ShaderStage* - the ShaderStage object for this ShaderPss
 */
typedef std::tuple<renderStage, unsigned int, ShaderStage*> ShaderPass;

typedef int (*renderCallBack)();

typedef std::tuple<renderStage, int, renderCallBack> callback;
typedef std::tuple<renderStage, GLuint, GLuint> frameBufferObject;

class RenderPass
{
public:
    RenderPass();
    RenderPass(const char* f);
    RenderPass(std::string s);
    RenderPass(RenderPass const& r);
    RenderPass& operator=(RenderPass const&);
    ~RenderPass();
    /**
     * @brief Update what stage of the renderpass we are on
     *
     * @details This is not the same as a normal update loop function
     * This will check what stage of the renderpass we are on the progress to the next one
     *
     */
    void Update();

    /**
     * @brief Run the current shaderstage and move active to the next one
     *
     * @details
     *
     */
    void RunStage();
    /**
     * @brief Set the stage to a specific stage.
     *
     * @param s the name of the stage to set
     */
    void SetSpecificStage(std::string s);
    /**
     * @brief Bind a buffer.
     *
     * @param s the buffer's name
     */
    void BindBuffer(std::string s);
    /**
     * @brief Unbind a buffer.
     *
     * @param s the buffer to unbind
     */
    void UnBindBuffer(std::string s);
    /**
     * @brief Bind an FBO.
     *
     * @param id the FBO to bind
     */
    void BindActiveFBO(int id);
    /**
     * @brief reset the active FBO.
     *
     */
    void UnBindActiveFBO();
    /**
     * @brief Write data to a buffer.
     *
     * @param buffer the buffer name to write to
     * @param dataSize the size of the data
     * @param data the data to write
     */
    void WriteBuffer(std::string buffer, size_t dataSize, void* data);
    /**
     * @brief Write an attribute.
     *
     * @param buffer the attribute name to write
     * @param data the data
     */
    void WriteAttribute(std::string buffer, void* data);

    /**
     * @brief Flatten all active FBOs.
     * 
     */
    void FlattenFBOs();

    /**
     * @brief Register a callBack function for a specific renderstage.
     *
     * @param stage the stage to callback for
     * @param id the id of the
     * @param fn the function to call
     */
    void RegisterCallBack(renderStage stage, int id, renderCallBack fn);
    /**
     * @brief Get what stage we are on.
     *
     * @return the current render stage
     */
    renderStage CurrentStage();
    /**
     * @brief Get an FBO from the secondary buffers.
     *
     * @param s the FBO name
     * @return the Buffer ID
     */
    std::pair<GLuint, GLuint> GetFrameBuffer(std::string);
    /**
     * @brief Get an FBO based on ID.
     *
     * @param id the id
     * @return the buffer id
     */
    std::pair<GLuint, GLuint> GetFrameBuffer(int id);
    /**
     * @brief Get reference to array of primary Frame buffer objects.
     * 
     * @return 
     */
    std::array<frameBufferObject, 3> const& GetPrimaryFBOs();
    /**
     * @brief Get reference to array of secondaru Frame buffer objects.
     *
     * @return
     */
    std::array<frameBufferObject, 3> const& GetSecondaryFBOs();
    /**
     * @brief Clear all FBOs and reset stage.
     * 
     */
    void ResetRender();
    /**
     * @brief Resize all FBOs.
     * 
     */
    void ResizeFBOs();
    /**
     * @brief Resize a specific FBO identified by name.
     * 
     * @param 
     * @param newSize
     */
    void ResizeSpecificFBO(std::string, glm::vec2 const& newSize);

    bool QuerryAttribute(std::string);

    bool QuerryStage(std::string);

    int MakeFBO(std::string);

    std::string MakeVAO(std::string);

    void DispatchCompute(int x, int y, int z);
private:

  void SetupDefaultFBOs();

    // The primary render and the secondary render each get 3 FBOs, assigned to be BG, FG and UI.
    std::array<frameBufferObject, 3> _primaryFBOs;
    std::array<frameBufferObject, 3> _secondaryFBOs;
    // The rpass.meta file can add more FBOs
    std::map<std::string, frameBufferObject> _additionalFBOs;

    std::unordered_map<std::string, ShaderPass> _passess;

    std::vector<callback> _renderCallbacks;
    ShaderPass _activeShaderStage;
    renderStage _activeStage;
    ShaderStage* _flattenStage;
};
