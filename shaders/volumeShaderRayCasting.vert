#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;

out vec3 texCoord;

void main()
{
    texCoord = aTex;
    gl_Position = vec4(aPos, 1.0);
}
