char const* fmerge_vert = "#version 430\
\
layout(location = 0)in vec2 pos;\
layout(location = 2)in vec2 texPos;\
\
layout(location = 0)out vec2 texCoordinates;\
\
void main()\
{\
  gl_Position = vec4(pos, 0, 1);\
  texCoordinates = texPos;\
}";
