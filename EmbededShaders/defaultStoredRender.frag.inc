char const* defaultStoredRender_frag = "#version 450\n\
layout(location = 0) in vec2 texPos;\n\
layout(location = 1) in vec4 color;\n\
layout(location = 2) in vec4 worldNormal;\n\
layout(location = 3) in vec4 worldPosition;\n\
layout(location = 4) in flat int InstanceID;\n\
uniform vec4 eye_position = vec4(0, 0, 0, 1);\n\
uniform sampler2D tex;\n\
uniform vec4 light_position = vec4(0, 0, 0, 1);\n\
uniform vec3 light_color = vec3(1, 1, 1);\n\
uniform int textured = 0;\n\
uniform int enableLighting = 0;\n\
out vec4 diffuseColor;\n\
\n\
struct buff {\n\
  mat4 matrix;\n\
  mat4 normalMatrix;\n\
  vec3 color;\n\
  int materialID;\n\
};\n\
\n\
struct material{\n\
  vec3 diffuse;\n\
  vec3 specular;\n\
  float specular_exponent;\n\
};\n\
\n\
layout(std430, binding = 0) buffer RenderBuffer \n\
{ \n\
  buff data[]; \n\
};\n\
\n\
layout(std430, binding = 1) buffer MaterialBuffer {\n\
  material materials[];\n\
};\n\
\n\
void main() {\n\
  int instance = InstanceID;\n\
  buff b = data[instance];\n\
  if (enableLighting == 0) {\n\
    diffuseColor = vec4(b.color, 1);\n\
    if (textured == 1)\n\
      diffuseColor *= texture(tex, texPos);\n\
  } else {\n\
    material mi = materials[b.materialID];\n\
    vec3 ambient = mi.diffuse * b.color;\n\
    vec4 m = normalize(worldNormal);\n\
    vec4 L = normalize(light_position - worldPosition);\n\
    vec4 V = normalize(eye_position - worldPosition);\n\
    float ml = dot(m, L);\n\
    vec3 diffuse = mi.diffuse * light_color;\n\
    float difMult = max(ml, 0);\n\
    diffuse *= difMult;\n\
    vec3 specular = mi.specular * light_color;\n\
    vec4 R = 2 * ml * m - L;\n\
    float specMult = max(dot(R, V), 0);\n\
    if (specMult > 0 && mi.specular_exponent > 0)\n\
      specMult = pow(specMult, mi.specular_exponent);\n\
    specular *= specMult;\n\
    diffuseColor = vec4(specular + diffuse + ambient, 1);\n\
    if (textured == 1)\n\
      diffuseColor *= texture(tex, texPos);\n\
  }\n\
}";
