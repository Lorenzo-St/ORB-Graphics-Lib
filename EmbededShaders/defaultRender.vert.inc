char const* defaultRender_vert = "#version 450\n\
layout(location = 0) in vec4 pos;\n\
layout(location = 1) in vec4 vecColor;\n\
layout(location = 3) in vec2 texcoord;\n\
layout(location = 2) in vec4 normal;\n\
layout(location = 0) out vec2 texPos;\n\
layout(location = 1) out vec4 color;\n\
layout(location = 2) out vec4 worldNormal;\n\
layout(location = 3) out vec4 worldPosition;\n\
uniform mat4 objectMatrix;\n\
uniform mat4 screenMatrix;\n\
uniform mat4 normalMatrix;\n\
uniform float zoom;\n\
void main() {\n\
  worldPosition = objectMatrix * pos * zoom;\n\
  worldNormal = normalMatrix * normal;\n\
  gl_Position = screenMatrix * worldPosition;\n\
  texPos = texcoord;\n\
  color = vecColor;\n\
}";
