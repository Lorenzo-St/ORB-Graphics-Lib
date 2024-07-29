#version 450
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 vecColor;
layout(location = 3) in vec2 texcoord;
layout(location = 2) in vec4 normal;
struct buff {
  mat4 matrix;
  mat4 normalMatrix;
  vec3 color;
  int materialID;
};
layout(std430, binding = 0) buffer RenderBuffer { buff data[]; };
uniform mat4 screenMatrix;
uniform float zoom;
void main() {
  int instance = gl_InstanceID;
  buff b = data[instance];
  gl_Position = screenMatrix * b.matrix * pos * zoom;
}