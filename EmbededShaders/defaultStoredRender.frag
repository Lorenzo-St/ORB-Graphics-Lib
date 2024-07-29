#version 450
layout(location = 0) in vec2 texPos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 worldNormal;
layout(location = 3) in vec4 worldPosition;
layout(location = 4) in flat int InstanceID;
uniform vec4 eye_position = vec4(0, 0, 0, 1);
uniform sampler2D tex;
uniform vec4 light_position = vec4(0, 0, 0, 1);
uniform vec3 light_color = vec3(1, 1, 1);
uniform int textured = 0;
uniform int enableLighting = 0;
out vec4 diffuseColor;

struct buff {
  mat4 matrix;
  mat4 normalMatrix;
  vec3 color;
  int materialID;
};

struct material{
  vec3 diffuse;
  vec3 specular;
  float specular_exponent;
};

layout(std430, binding = 0) buffer RenderBuffer 
{ 
  buff data[]; 
};

layout(std430, binding = 1) buffer MaterialBuffer {
  material materials[];
};

void main() {
  int instance = InstanceID;
  buff b = data[instance];
  if (enableLighting == 0) {
    diffuseColor = vec4(b.color, 1);
    if (textured == 1)
      diffuseColor *= texture(tex, texPos);
  } else {
    material mi = materials[b.materialID];
    vec3 ambient = mi.diffuse * b.color;
    vec4 m = normalize(worldNormal);
    vec4 L = normalize(light_position - worldPosition);
    vec4 V = normalize(eye_position - worldPosition);
    float ml = dot(m, L);
    vec3 diffuse = mi.diffuse * light_color;
    float difMult = max(ml, 0);
    diffuse *= difMult;
    vec3 specular = mi.specular * light_color;
    vec4 R = 2 * ml * m - L;
    float specMult = max(dot(R, V), 0);
    if (specMult > 0 && mi.specular_exponent > 0)
      specMult = pow(specMult, mi.specular_exponent);
    specular *= specMult;
    diffuseColor = vec4(specular + diffuse + ambient, 1);
    if (textured == 1)
      diffuseColor *= texture(tex, texPos);
  }
}