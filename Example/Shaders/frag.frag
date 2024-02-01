#version 430
struct lightSource
{
    vec2 pos; // 8  |
    vec2 lightingSideVectorA;
    vec2 lightingSideVectorB;
    float sideAMag;
    float sideBMag;
    float sidesDot;
    float angle;
    float intensity; // 4  | could potentially make color and intensity not be float
    int angled;
}; 
layout(location = 0) in vec4 color;
layout(location = 4) in vec2 texCoords;
layout(location = 6) in vec4 worldCoords;
layout(location = 0) out vec4 diffuseColor;

layout(location = 25) uniform sampler2D tex;
layout(location = 26) uniform int textured;
layout(location = 27) uniform vec4 globalColor;
layout(location = 31) uniform mat4 texMulti;
layout(location = 47) uniform int sourceCount;
layout(location = 48) uniform int enableLighting;
const float gamma = 0.00025;
const float hazeScale = .125;
layout(std430, binding=1) buffer sources
{
    lightSource boundSources[];
};

float LightCast(in int id)
{
            lightSource temp = boundSources[id];
            vec2 pos = worldCoords.xy;
            vec2 dis = (temp.pos - pos);
            float mag = length(dis);


            float dotA = dot(dis, temp.lightingSideVectorA);
            float dotB = dot(dis, temp.lightingSideVectorB);
            
            float angleA = acos(dotA/(temp.sideAMag * mag));
            float angleB = acos(dotB/(temp.sideBMag * mag));
            float sum = (angleA+angleB) - temp.angle * temp.angled;
            float val = sum * temp.angled;
            float inten = (temp.intensity / (dot(dis, dis))) * (1-(val));
            inten = clamp(inten, 0. ,1.);
            return inten;
}


float checkLights()
{
    float lowest = 0;
    for(int i = 0; i < sourceCount; ++i)
    {
        float cas = LightCast(i);
        float bigger = step(lowest, cas);
        lowest = (cas * bigger) + (lowest * (1.0-bigger));
    }
    return clamp(lowest, 0.0, 1.0);
}




void main() {
		vec2 texCoord = vec2(texMulti * vec4(texCoords, 0, 1));


  vec4 pre = vec4(0);
	if(textured == 1)
		pre = color * texture(tex, texCoord);
	else 
		pre = color * globalColor;
 
 if(enableLighting == 1)
 {
    float lighting = checkLights();
    diffuseColor = pre * vec4(lighting,lighting,lighting,1);
 }
 else
 {
    diffuseColor = pre;
 }

} 
