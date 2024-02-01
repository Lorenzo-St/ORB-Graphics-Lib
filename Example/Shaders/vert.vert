#version 430


layout(location = 0) in vec2 Pos;

layout(location = 2) in vec4 Color;
layout(location = 6) in vec2 texCords;

layout(location = 8) uniform mat4 objectMatrix;
layout(location = 12) uniform mat4 CamMatrix;
layout(location = 16) uniform mat4 WorldMatrix;
layout(location = 20) uniform float zoom;

layout(location = 0) out vec4 color; 
layout(location = 4) out vec2 texCoords;
layout(location = 6) out vec4 worldCoords;
void main() {
	worldCoords = objectMatrix * vec4(Pos,0.0, 1);
	gl_Position = WorldMatrix * CamMatrix * worldCoords * zoom;
	color = Color;
	texCoords = texCords;
}
