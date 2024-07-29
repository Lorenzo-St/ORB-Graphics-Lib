char const* flatten_vert = "#version 450\n\
layout(location = 0) in vec2 pos;\n\
layout(location = 2) in vec2 texPos;\n\
layout(location = 0) out vec2 texCoordinates;\n\
void main() {\n\
  gl_Position = vec4(pos, 0, 1);\n\
  texCoordinates = texPos;\n\
}";
