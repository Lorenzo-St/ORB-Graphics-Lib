char const* flatten_frag = "#version 450\n\
uniform sampler2D FBO;\n\
uniform sampler2DArray FBOArray;\n\
uniform int array;\n\
layout(location = 0) in vec2 texCoordinates;\n\
out vec4 diffuseColor;\n\
const float gamma = 0.0025;\n\
void main() {\n\
  vec4 c = texture(FBO, texCoordinates);\n\
  if (c.a > gamma)\n\
    diffuseColor = c;\n\
  else\n\
    diffuseColor = vec4(0, 0, 0, 0);\n\
}\n\
";
