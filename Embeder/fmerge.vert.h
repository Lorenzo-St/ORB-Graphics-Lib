char const* fmerge.vert = "
#version 430
\n
\nlayout(location = 0)in vec2 pos;
\nlayout(location = 2)in vec2 texPos;
\n
\nlayout(location = 0)out vec2 texCoordinates;
\n
\nvoid main()
\n{
\n  gl_Position = vec4(pos, 0, 1);
\n  texCoordinates = texPos;
\n}";
