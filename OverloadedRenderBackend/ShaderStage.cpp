/*********************************************************************
 * @file   ShaderStage.cpp
 * @brief  Shader Stage implementation
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   October 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *********************************************************************/
#include "pch.h"

#include "ShaderStage.h"

 // This is a local only thing, the library used here is technically not allowed, so it is only on my
 // local stuff
constexpr int ENABLE_SHADER_PRINTF = 0;
#ifdef ASTRO_ENABLE_SHADER_PRINTF
#include "../ShaderPrintf/shaderprintf.h"
#endif
#include "Stream.h"

#include "ShaderLog.hpp"
void CheckError(int i);

static std::vector<GLchar>& loadFile(const char* fileName)
{
  static std::vector<GLchar> file;
  file.clear();
  FILE* f;
  fopen_s(&f, fileName, "rb");
  if (f == nullptr)
    throw std::runtime_error("File not opened correctly, likely bad path");
  size_t length = 0;
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  file.resize(length + 1);
  fseek(f, 0, 0);
  fread(file.data(), length, 1, f);
  fclose(f);
  return file;
}
#ifdef ASTRO_ENABLE_SHADER_PRINTF

GLuint createShader(const std::string& path, GLenum shaderType)
{

  using namespace std;
  auto& file = loadFile(path.c_str());
  string source = string(file.begin(), file.end());
  GLuint shader = glCreateShader(shaderType);
  auto ptr = (const GLchar*)source.c_str();

  glShaderSourcePrint(shader, 1, &ptr, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    string log(length + 1, '\0');
    glGetShaderInfoLog(shader, length + 1, &length, &log[0]);
    printf("log of compiling %s:\n%s\n", path.c_str(), log.c_str());
    return 0;
  }
  return shader;
}
#endif
GLuint ShaderStage::CreateShader(GLenum type, const char* filepath)
{
  auto& frag = loadFile(filepath);
  GLuint result = glCreateShader(type);
  auto p = frag.data();
  glShaderSource(result, 1, &p, nullptr);
  glCompileShader(result);

  int linkok = 0;
  glGetShaderiv(result, GL_COMPILE_STATUS, &linkok);
  if (linkok == 0)
  {
    char buffer[1000];
    GLsizei len;
    glGetShaderInfoLog(result, _countof(buffer), &len, buffer);
    Log(Error, "Compile Failed: ", buffer);
    throw std::runtime_error(buffer);
  }

  Log(Message, "Shader compiled correctly: ", filepath);
  return result;
}

bool ShaderStage::hasStage(shaderStages s)
{
  return (_activeShaders & static_cast<int>(s)) != 0;
}

void ShaderStage::InitializeShaderProgram()
{

  // Read that section
  // Loop the input attributes
  size_t totalSize = 0;
  for (auto& in : _inputAttributes)
  {
    glBindAttribLocation(_program, in.second.first, in.first.c_str());
    totalSize += in.second.second;
  }
  glLinkProgram(_program);

  assert(glIsProgram(_program));
  GLint linkok = 0;
  glGetProgramiv(_program, GL_LINK_STATUS, &linkok);
  if (linkok == 0)
  {
    char buffer[1000];
    GLsizei len;
    glGetProgramInfoLog(_program, _countof(buffer), &len, buffer);
    Log(Error, "Linking Failed: ", buffer);
    throw std::runtime_error(buffer);
  }
  glUseProgram(_program);
  assert(linkok == 1 && "Link was not ok");
  if (hasStage(shaderStages::vertex))
  {
    GLuint temp;
    glGenVertexArrays(1, &temp);
    _buffers["VAO"] = { temp, GL_ARRAY_BUFFER_BINDING };

    glGenBuffers(1, &temp);
    _buffers["VBO"] = { temp, GL_ARRAY_BUFFER };

    glBindVertexArray(_buffers["VAO"].first);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers["VBO"].first);
    for (auto& in : _inputAttributes)
    {
      glEnableVertexAttribArray(in.second.first);
    }
    size_t offset = 0;

    for (auto& in : _inputAttributes)
    {
      glVertexAttribPointer(
        in.second.first,
        static_cast<GLint>(in.second.second),
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(totalSize) * sizeof(float),
        reinterpret_cast<void*>(offset * sizeof(float)));
      offset += in.second.second;
      CheckError(__LINE__);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  for (auto& uni : _uniformAttributes)
  {
    uni.second.first = glGetUniformLocation(_program, uni.first.c_str());
    CheckError(__LINE__);
    Log(Message, "Recieved Uniform", uni.first, "location:", uni.second.first);
  }

}

void ShaderStage::CleanUp()
{
  for (auto& b : _buffers)
  {
    if (b.second.second == GL_ARRAY_BUFFER_BINDING)
      glDeleteVertexArrays(1, &b.second.first);
    else
      glDeleteVertexArrays(1, &b.second.first);
  }
  _buffers.clear();
  _inputAttributes.clear();
  _uniformAttributes.clear();
  glDeleteProgram(_program);
}

void ShaderStage::Dispatch(int x, int y, int z)
{
#ifdef ASTRO_ENABLE_SHADER_PRINTF
  GLuint printBuffer;

  if constexpr (ENABLE_SHADER_PRINTF)
  {
    printBuffer = createPrintBuffer();
    bindPrintBuffer(_program, printBuffer);
  }
#endif
  if (hasStage(shaderStages::compute))
    glDispatchCompute(x, y, z);
#ifdef ASTRO_ENABLE_SHADER_PRINTF

  if constexpr (ENABLE_SHADER_PRINTF)
  {
    Log(Message, "GLSL Print:", getPrintBufferString(printBuffer).c_str());
    deletePrintBuffer(printBuffer);
  }
#endif
}

GLuint ShaderStage::QueryUniformBinding(std::string uniform)
{
  return _uniformAttributes[uniform].first;
}

GLuint ShaderStage::QueryBufferID(std::string buffer)
{
  return _buffers[buffer].first;
}

std::string ShaderStage::MakeExtraVAO(std::string name)
{
  GLuint temp;
  size_t totalSize = 0;
  for (auto& in : _inputAttributes)
  {
    totalSize += in.second.second;
  }
  glGenVertexArrays(1, &temp);
  _buffers[name] = { temp, GL_ARRAY_BUFFER_BINDING };

  glBindVertexArray(_buffers[name].first);
  glBindBuffer(GL_ARRAY_BUFFER, _buffers["VBO"].first);
  for (auto& in : _inputAttributes)
  {
    glEnableVertexAttribArray(in.second.first);
  }
  CheckError(__LINE__);
  size_t offset = 0;

  for (auto& in : _inputAttributes)
  {
    glVertexAttribPointer(
      in.second.first,
      static_cast<GLint>(in.second.second),
      GL_FLOAT,
      GL_FALSE,
      static_cast<GLsizei>(totalSize) * sizeof(float),
      reinterpret_cast<void*>(offset * sizeof(float)));
    offset += in.second.second;
    CheckError(__LINE__);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  CheckError(__LINE__);
  return name;
}

ShaderStage::~ShaderStage()
{
  if (!keepAlive)
    CleanUp();
}

ShaderStage::ShaderStage(int version)
{
  enum class VERSIONS : int
  {
    DEFAULT_RENDER,
    FLATTEN,

  };
  _program = glCreateProgram();

  switch (static_cast<VERSIONS>(version))
  {
  case VERSIONS::DEFAULT_RENDER:
  {
    const char* vert = "#version 450\n\
    layout(location = 0) in vec4 pos;\
    layout(location = 4) in vec4 vecColor;\
    layout(location = 8) in vec2 texcoord;\
    layout(location = 0) out vec2 texPos;\
    layout(location = 2) out vec4 color;\
    uniform mat4 objectMatrix;\
    uniform mat4 screenMatrix;\
    void main()\
    {\
      gl_Position = screenMatrix * objectMatrix * pos;\
      texPos = texcoord;\
      color = vecColor;\
    }";
    const char* const frag = "#version 450\n\
    layout(location = 0) in vec2 texPos;\
    layout(location = 2) in vec4 color;\
    uniform sampler2D tex;\
    uniform int textured;\
    uniform vec4 globalColor;\
    out vec4 diffuseColor;\
    void main()\
    {\
      diffuseColor = color * globalColor;\
      if (textured == 1)\
        diffuseColor *= texture(tex, texPos);\
    }\
    ";
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER), vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragmentShader, 1, &frag, nullptr);
    glShaderSource(vertexShader, 1, &vert, nullptr);
    glCompileShader(vertexShader);
    int linkok = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &linkok);
    if (linkok == 0)
    {
      char buffer[1000];
      GLsizei len;
      glGetShaderInfoLog(vertexShader, _countof(buffer), &len, buffer);
      Log(Error, "Compile Failed: ", buffer);
      throw std::runtime_error(buffer);
    }
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &linkok);
    if (linkok == 0)
    {
      char buffer[1000];
      GLsizei len;
      glGetShaderInfoLog(fragmentShader, _countof(buffer), &len, buffer);
      Log(Error, "Compile Failed: ", buffer);
      throw std::runtime_error(buffer);
    }
    glAttachShader(_program, fragmentShader);
    glAttachShader(_program, vertexShader);
    _inputAttributes["pos"] = { 0,4 };
    _inputAttributes["vecColor"] = { 4,4 };
    _inputAttributes["texcoord"] = { 8,2 };
    //_uniformAttributes[name] = { 0, size };
    _uniformAttributes["objectMatrix"] = { 0, 64 };
    _uniformAttributes["screenMatrix"] = { 0, 64 };
    _uniformAttributes["tex"] = { 0, ULLONG_MAX };
    _uniformAttributes["textured"] = { 0, 1 };
    _uniformAttributes["globalColor"] = { 0, 16 };

    _activeShaders |= static_cast<int>(shaderStages::fragment) | static_cast<int>(shaderStages::vertex);

    assert(glGetError() == 0 && "An error has occured here");
  }
  break;
  case VERSIONS::FLATTEN:
  {
    const char* vert = "\
    #version 450\n\
    layout(location = 0)in vec2 pos;\
    layout(location = 2)in vec2 texPos;\
    layout(location = 0)out vec2 texCoordinates;\
    void main()\
    {\
      gl_Position = vec4(pos, 0, 1);\
      texCoordinates = texPos;\
    }";
    const char* const frag = "\
    #version 450\n\
    uniform sampler2D FBO;\
    uniform sampler2DArray FBOArray;\
    uniform int array;\
    layout(location = 0)in vec2 texCoordinates;\
    out vec4 diffuseColor;\
    const float gamma = 0.0025;\
    void main()\
    {\
      vec4 c = texture(FBO, texCoordinates);\
      if (c.a > gamma)\
        diffuseColor = c;\
      else\
        diffuseColor = vec4(0, 0, 0, 0);\
    }\
    ";
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER), vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragmentShader, 1, &frag, nullptr);
    glShaderSource(vertexShader, 1, &vert, nullptr);
    glCompileShader(vertexShader);
    int linkok = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &linkok);
    if (linkok == 0)
    {
      char buffer[1000];
      GLsizei len;
      glGetShaderInfoLog(vertexShader, _countof(buffer), &len, buffer);
      Log(Error, "Compile Failed: ", buffer);
      throw std::runtime_error(buffer);
    }
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &linkok);
    if (linkok == 0)
    {
      char buffer[1000];
      GLsizei len;
      glGetShaderInfoLog(fragmentShader, _countof(buffer), &len, buffer);
      Log(Error, "Compile Failed: ", buffer);
      throw std::runtime_error(buffer);
    }
    glAttachShader(_program, fragmentShader);
    glAttachShader(_program, vertexShader);
    _uniformAttributes["FBO"] = { 0, ULLONG_MAX };
    _uniformAttributes["FBOArray"] = { 0, ULLONG_MAX };
    _uniformAttributes["array"] = { 0, 1 };

    _inputAttributes["pos"] = { 0,2 };
    _inputAttributes["texPos"] = { 2,2 };


    _activeShaders |= static_cast<int>(shaderStages::fragment) | static_cast<int>(shaderStages::vertex);

    assert(glGetError() == 0 && "An error has occured here");

  }
  break;
  }
  InitializeShaderProgram();



}

ShaderStage::ShaderStage(std::string path)
{
  Stream file("./Managed/shaders/" + path);
  if (file.Open() == false)
  {
    Log(Error, "Bad FIle Path:", path);
    throw std::invalid_argument("Bad file path");
  }
  // Read each line and check for <
  std::string token;
  _program = glCreateProgram();
  std::vector<GLuint> shaders;
  while (file.isEOF() != true)
  {
    // if we fine a < then set what section we are reading
    token = makeLowerCase(file.readString());
    if (token.find('<') != std::string::npos)
    {
      //
      if (token == "<vertex>")
      {
        _activeShaders |= static_cast<int>(shaderStages::vertex);

        GLuint vert = CreateShader(
          GL_VERTEX_SHADER, ("./Managed/shaders/" + file.readString()).c_str());
        // Create the shader
        shaders.push_back(vert);
        assert(glIsShader(vert) && "Shader was not created properly");
        glAttachShader(_program, vert);
        Log(Message, "Created Vertex Stage");
      }
      else if (token == "<fragment>")
      {
        _activeShaders |= static_cast<int>(shaderStages::fragment);
        GLuint frag = CreateShader(
          GL_FRAGMENT_SHADER, ("./Managed/shaders/" + file.readString()).c_str());
        assert(glIsShader(frag) && "Shader was not created properly");
        glAttachShader(_program, frag);
        shaders.push_back(frag);
        Log(Message, "Created Fragment Stage");
      }
      else if (token == "<compute>")
      {
        if (hasStage(shaderStages::fragment) || hasStage(shaderStages::vertex))
        {
          throw std::invalid_argument("Cannot have compute shader in graphics pipeling");
        }
        if constexpr (ENABLE_SHADER_PRINTF)
        {

#ifdef ASTRO_ENABLE_SHADER_PRINTF
          _activeShaders |= static_cast<int>(shaderStages::compute);
          GLuint com =
            createShader("./Managed/shaders/" + file.readString(), GL_COMPUTE_SHADER);
          assert(glIsShader(com) && "Shader was not created properly");
          glAttachShader(_program, com);
          shaders.push_back(com);
          Log(Message, "Created Compute Stage: PRINTF ENABLED");
#endif
        }
        else
        {
          _activeShaders |= static_cast<int>(shaderStages::compute);
          GLuint com = CreateShader(
            GL_COMPUTE_SHADER, ("./Managed/shaders/" + file.readString()).c_str());
          assert(glIsShader(com) && "Shader was not created properly");
          glAttachShader(_program, com);
          shaders.push_back(com);
          Log(Message, "Created Compute Stage: PRINTF DISABLED");
        }
      }
      else if (token == "<in>")
      {
        while (true)
        {

          token = file.readString();
          if (makeLowerCase(token) == "</in>")
            break;
          // Find where the equal sign is
          size_t bracket = token.find('[');

          // Get the name
          std::string name = token.substr(0, bracket);
          // Erase the name and the equal sign
          token.erase(token.begin(), token.begin() + bracket + 1);
          size_t size = std::stoi(token);
          size_t equalSign = token.find('=');
          token.erase(token.begin(), token.begin() + equalSign + 1);
          // Get the position
          GLuint pos = std::stoi(token);
          // Save it
          _inputAttributes[name] = { pos, size };
          Log(Message, "Added Attribute:", name, "at location:", pos, "to Shader:", path);
        }
      }
      else if (token == "<uniform>")
      {
        while (true)
        {

          token = file.readString();
          if (makeLowerCase(token) == "</uniform>")
            break;
          // Find the size part of the uniform
          size_t equalSign = token.find('[');
          // get the name
          std::string name = token.substr(0, equalSign);
          // Erase the name
          token.erase(token.begin(), token.begin() + equalSign + 1);
          if (makeLowerCase(token).find("unique") != std::string::npos)
          {
            // "Unique" identifies a texture location
            // Therefore we don't have a specified size as we have to use a different
            // way to bind it SO set the size to be ULLONG_MAX as like an identifier
            // number
            _uniformAttributes[name] = { 0, ULLONG_MAX };
          }
          else
          {
            GLuint size = std::stoi(token);
            _uniformAttributes[name] = { 0, size };
          }

          Log(Message, "Added Uniform:", name, "to Shader:", path);
        }
      }
      else if (token == "<buffers>")
      {
        while (true)
        {
          token = file.readString();
          if (makeLowerCase(token) == "</buffers>")
            break;
          // find first bracket
          size_t bracket = token.find('[');
          // Get name
          std::string name = token.substr(0, bracket);
          // Erase the name
          token.erase(token.begin(), token.begin() + bracket + 1);
          int type = std::stoi(token);
          GLuint newBuffer = 0;
          glGenBuffers(1, &newBuffer);
          _buffers[name] = { newBuffer, bufferTypes.at(type) };
          Log(Message, "Added buffer:", name, "to Shader:", path);
        }
      }
    }
  }
  InitializeShaderProgram();
  for (auto& s : shaders)
  {
    glDeleteShader(s);
  }
}

ShaderStage::ShaderStage(const char* path) : ShaderStage(std::string(path))
{
}

ShaderStage::ShaderStage(ShaderStage& s)
  : _uniformAttributes(s._uniformAttributes), _inputAttributes(s._inputAttributes),
  _buffers(s._buffers), _activeShaders(s._activeShaders), _program(s._program)
{
  s.keepAlive = true;
}

ShaderStage& ShaderStage::operator=(ShaderStage const& s)
{
  _uniformAttributes = s._uniformAttributes;
  _inputAttributes = s._inputAttributes;
  _buffers = s._buffers;
  _activeShaders = s._activeShaders;
  _program = s._program;
  const_cast<ShaderStage&>(s).keepAlive = true;
  return *this;
}

void ShaderStage::WriteAttribute(std::string s, void* data)
{
  glUseProgram(_program);
  auto& attrib = _uniformAttributes[s];
  switch (attrib.second)
  {
  case ULLONG_MAX:
    glUniform1i(attrib.first, *static_cast<int*>(data));
    break;
  case 64:
    glUniformMatrix4fv(attrib.first, 1, false, reinterpret_cast<const GLfloat*>(data));
    break;
  case 16:
    glUniform4fv(attrib.first, 1, reinterpret_cast<const GLfloat*>(data));
    break;
  case 81:
    glUniform2iv(attrib.first, 1, reinterpret_cast<const GLint*>(data));
    break;
  case 80:
  case 8:
    glUniform2fv(attrib.first, 1, reinterpret_cast<const GLfloat*>(data));
    break;
  case 41:
    glUniform1i(attrib.first, *reinterpret_cast<const GLint*>(data));
    break;
  case 40:
  case 4:
    glUniform1f(attrib.first, *reinterpret_cast<const GLfloat*>(data));
    break;
  case 1:
    glUniform1i(attrib.first, *reinterpret_cast<int*>(data));
    break;
  }
  // CheckError(__LINE__);
}

void ShaderStage::WriteBuffer(std::string s, size_t dataSize, void* data)
{
  auto& bufferObject = _buffers[s];
  glBufferData(bufferObject.second, dataSize, data, GL_STATIC_DRAW);
}

void ShaderStage::BindBuffer(std::string s)
{
  auto& bufferObject = _buffers[s];
  if (bufferObject.second == GL_ARRAY_BUFFER_BINDING)
    glBindVertexArray(bufferObject.first);
  else
    glBindBuffer(bufferObject.second, bufferObject.first);
}

void ShaderStage::UnBindBuffer(std::string s)
{
  auto& bufferObject = _buffers[s];
  if (bufferObject.second == GL_ARRAY_BUFFER_BINDING)
    glBindVertexArray(0);
  else
    glBindBuffer(bufferObject.second, 0);
}

void ShaderStage::SetActive(void)
{
  glUseProgram(_program);
}

bool ShaderStage::QuerryAttribute(std::string s)
{
  return _uniformAttributes.find(s) != _uniformAttributes.end();
}

inline GLuint ShaderStage::Program()
{
  return _program;
}
