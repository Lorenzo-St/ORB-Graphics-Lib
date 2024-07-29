#version 450
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 vecColor;
layout(location = 3) in vec2 texcoord;
layout(location = 2) in vec4 normal;
layout(location = 0) out vec2 texPos;
layout(location = 1) out vec4 color;
layout(location = 2) out vec4 worldNormal;
layout(location = 3) out vec4 worldPosition;
layout(location = 4) out flat int InstanceID;
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
  InstanceID = instance;
  buff b = data[instance];
  worldPosition = b.matrix * pos * zoom;
  worldNormal = b.normalMatrix * normal;
  gl_Position = screenMatrix * worldPosition;
  texPos = texcoord;
  color = vecColor;
}