/*********************************************************************
 * @file   ShaderStage.h
 * @brief  Shader stage struct definition
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   October 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#pragma once

#include <glad.h>
#include <unordered_map>

// Read in the meta file
// load the shaders and create the program
// load and store each of the in locations and its size
// load and store each of the uniform variable locations and their size

// This defines a bit mask we will use later to determine what we need to create for the shader
enum class shaderStages : long
{
    vertex = 1 << 0,
    fragment = 1 << 1,
    compute = 1 << 2,
    geometry = 1 << 3,
};

const std::unordered_map<int, GLenum> bufferTypes = {
    {0, GL_ARRAY_BUFFER},
    {1, GL_ATOMIC_COUNTER_BUFFER},
    {2, GL_COPY_READ_BUFFER},
    {3, GL_COPY_WRITE_BUFFER},
    {4, GL_DISPATCH_INDIRECT_BUFFER},
    {5, GL_ELEMENT_ARRAY_BUFFER},
    {6, GL_PIXEL_PACK_BUFFER},
    {7, GL_PIXEL_UNPACK_BUFFER},
    {8, GL_QUERY_BUFFER},
    {9, GL_SHADER_STORAGE_BUFFER},
    {10, GL_TEXTURE_BUFFER},
    {11, GL_TRANSFORM_FEEDBACK_BUFFER},
    {12, GL_UNIFORM_BUFFER}};

// size is in bytes
typedef std::pair<GLuint, size_t> shaderAttribute;
typedef std::pair<GLuint, GLenum> shaderBuffer;
class RenderPass;
class ShaderStage
{
public:
    ~ShaderStage();
    ShaderStage() = default;
    /**
     * @brief Special Ctor to create a ShaderStage with standardized default parameters. This is not to be called from anywhere but RenderPass
     * 
     * @param int - will be passed into a switch statement
     */
    ShaderStage(int);
    /**
     * @brief Load a shader stage
     *
     * @param path std::string version
     */
    ShaderStage(std::string path);
    /**
     * @brief Load a shader stage
     *
     * @param path const char* version
     */
    ShaderStage(const char* path);
    ShaderStage(ShaderStage& s);
    ShaderStage& operator=(ShaderStage const&);
    /**
     * @brief Write data to the shader stage.
     *
     * @param s the attribute to write to
     * @param data pointer to the data to write
     */
    void WriteAttribute(std::string s, void* data);

    /**
     * @brief Write data to a buffer
     *
     * @param s the buffer to write to
     * @param dataSize the size of the data
     * @param data the data to write
     */
    void WriteBuffer(std::string s, size_t dataSize, void* data);

    void WriteSubBufferData(std::string, int index, size_t structSize, void* data);
    void SetBufferBase(std::string buffer, int base);


    /**
     * @brief Bind a buffer
     *
     * @param s the bufer to bind
     */
    void BindBuffer(std::string s);

    /**
     * @brief
     *
     * @param s
     */
    void UnBindBuffer(std::string s);

    /**
     * @brief Set this shader stage as the active program
     *
     */
    void SetActive(void);

    bool QuerryAttribute(std::string);
    /**
     * @brief Get the program id
     *
     * @return
     */
    GLuint Program();
    /**
     * @brief Release all buffers and objects assigned to this shader.
     *
     */
    void CleanUp();

    /**
     * @brief Dispatch a compute shader stage.
     *
     * @param x the x size
     * @param y the y size
     * @param z the z size
     */
    void Dispatch(int x, int y, int z);

    /**
     * @brief Query the location of a uniform.
     *
     * @param uniform the name of the unifrom
     * @return the binding of the uniform
     */
    GLuint QueryUniformBinding(std::string uniform);

    /**
     * @brief Query the id of a buffer.
     *
     * @param buffer the buffer to query
     * @return the buffer's id, 0 if buffer not exist
     */
    GLuint QueryBufferID(std::string buffer);

    // The parent of this renderpass
    RenderPass* parent;

    std::string MakeExtraVAO(std::string name);

    bool HasVAO(std::string name);
    bool HasBuffer(std::string name);
private:
    /**
     * @brief Create a shader from a file
     *
     * @param type the type of the shader
     * @param filepath the file path to the shader
     * @return the assigned id of the shader
     */
    GLuint CreateShader(GLenum type, const char* filepath);
    /**
     * @brief Check if the shader has a specific stage
     *
     * @return
     */
    bool hasStage(shaderStages s);

    void InitializeShaderProgram();

    // Using unordered map cause we dont care about order
    std::unordered_map<std::string, shaderAttribute> _uniformAttributes;
    std::unordered_map<std::string, shaderAttribute> _inputAttributes;
    std::unordered_map<std::string, shaderBuffer> _buffers;

    long _activeShaders = 0;
    bool keepAlive = false;
    GLuint _program;
};
