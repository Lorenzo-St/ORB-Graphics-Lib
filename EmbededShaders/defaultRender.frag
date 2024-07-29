#version 450
layout(location = 0) in vec2 texPos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 worldNormal;
layout(location = 3) in vec4 worldPosition;
uniform vec4 eye_position = vec4(0, 0, 0, 1);
uniform vec3 diffuse_coefficient = vec3(.5, .5, .5);
uniform vec3 specular_coefficient = vec3(.5, .5, .5);
uniform float specular_exponent = 1;
uniform sampler2D tex;
uniform vec4 light_position = vec4(0, 0, 0, 1);
uniform vec3 light_color = vec3(1, 1, 1);
uniform int textured = 0;
uniform int enableLighting = 0;
uniform vec4 globalColor;
out vec4 diffuseColor;
void main() {
  if (enableLighting == 0) {
    diffuseColor = color * globalColor;
    if (textured == 1)
      diffuseColor *= texture(tex, texPos);
  } else {
    vec3 ambient = diffuse_coefficient * globalColor.xyz;
    vec4 m = normalize(worldNormal);
    vec4 L = normalize(light_position - worldPosition);
    vec4 V = normalize(eye_position - worldPosition);
    float ml = dot(m, L);
    vec3 diffuse = diffuse_coefficient * light_color;
    float difMult = max(ml, 0);
    diffuse *= difMult;
    vec3 specular = specular_coefficient * light_color;
    vec4 R = 2 * ml * m - L;
    float specMult = max(dot(R, V), 0);
    if (specMult > 0 && specular_exponent > 0)
      specMult = pow(specMult, specular_exponent);
    specular *= specMult;
    diffuseColor = vec4(specular + diffuse + ambient, globalColor.w);
    if (textured == 1)
      diffuseColor *= texture(tex, texPos);
  }
}