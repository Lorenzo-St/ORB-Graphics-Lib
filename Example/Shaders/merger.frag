#version 430

uniform sampler2D FBO;
uniform sampler2DArray FBOArrray;
uniform int array;
layout(location=0)in vec2 texCoordinates;
out vec4 diffuseColor;

const float gamma = 0.0025; 

void main()
{
    vec4 c = texture(FBO, texCoordinates);
    if(c.a > gamma)
        diffuseColor = c;
    else
        diffuseColor = vec4(0,0,0,0);
}