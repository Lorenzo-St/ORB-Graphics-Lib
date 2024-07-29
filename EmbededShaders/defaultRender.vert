#version 450
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 vecColor;
layout(location = 3) in vec2 texcoord;
layout(location = 2) in vec4 normal;
layout(location = 0) out vec2 texPos;
layout(location = 1) out vec4 color;
layout(location = 2) out vec4 worldNormal;
layout(location = 3) out vec4 worldPosition;
uniform mat4 objectMatrix;
uniform mat4 screenMatrix;
uniform mat4 normalMatrix;
uniform float zoom;
void main() {
  worldPosition = objectMatrix * pos * zoom;
  worldNormal = normalMatrix * normal;
  gl_Position = screenMatrix * worldPosition;
  texPos = texcoord;
  color = vecColor;
}