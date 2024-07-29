char const* shadows_vert = "#version 450\n\
layout(location = 0) in vec4 pos;\n\
layout(location = 1) in vec4 vecColor;\n\
layout(location = 3) in vec2 texcoord;\n\
layout(location = 2) in vec4 normal;\n\
struct buff {\n\
  mat4 matrix;\n\
  mat4 normalMatrix;\n\
  vec3 color;\n\
  int materialID;\n\
};\n\
layout(std430, binding = 0) buffer RenderBuffer { buff data[]; };\n\
uniform mat4 screenMatrix;\n\
uniform float zoom;\n\
void main() {\n\
  int instance = gl_InstanceID;\n\
  buff b = data[instance];\n\
  gl_Position = screenMatrix * b.matrix * pos * zoom;\n\
}";
