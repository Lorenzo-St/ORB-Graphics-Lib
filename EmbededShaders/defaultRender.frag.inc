char const* defaultRender_frag = "#version 450\n\
layout(location = 0) in vec2 texPos;\n\
layout(location = 1) in vec4 color;\n\
layout(location = 2) in vec4 worldNormal;\n\
layout(location = 3) in vec4 worldPosition;\n\
uniform vec4 eye_position = vec4(0, 0, 0, 1);\n\
uniform vec3 diffuse_coefficient = vec3(.5, .5, .5);\n\
uniform vec3 specular_coefficient = vec3(.5, .5, .5);\n\
uniform float specular_exponent = 1;\n\
uniform sampler2D tex;\n\
uniform vec4 light_position = vec4(0, 0, 0, 1);\n\
uniform vec3 light_color = vec3(1, 1, 1);\n\
uniform int textured = 0;\n\
uniform int enableLighting = 0;\n\
uniform vec4 globalColor;\n\
out vec4 diffuseColor;\n\
void main() {\n\
  if (enableLighting == 0) {\n\
    diffuseColor = color * globalColor;\n\
    if (textured == 1)\n\
      diffuseColor *= texture(tex, texPos);\n\
  } else {\n\
    vec3 ambient = diffuse_coefficient * globalColor.xyz;\n\
    vec4 m = normalize(worldNormal);\n\
    vec4 L = normalize(light_position - worldPosition);\n\
    vec4 V = normalize(eye_position - worldPosition);\n\
    float ml = dot(m, L);\n\
    vec3 diffuse = diffuse_coefficient * light_color;\n\
    float difMult = max(ml, 0);\n\
    diffuse *= difMult;\n\
    vec3 specular = specular_coefficient * light_color;\n\
    vec4 R = 2 * ml * m - L;\n\
    float specMult = max(dot(R, V), 0);\n\
    if (specMult > 0 && specular_exponent > 0)\n\
      specMult = pow(specMult, specular_exponent);\n\
    specular *= specMult;\n\
    diffuseColor = vec4(specular + diffuse + ambient, globalColor.w);\n\
    if (textured == 1)\n\
      diffuseColor *= texture(tex, texPos);\n\
  }\n\
}";
