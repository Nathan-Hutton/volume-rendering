#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;

uniform mat4 mvp;

out vec3 texCoord;

void main()
{
    texCoord = aTex;
    gl_Position = mvp * vec4(aPos, 1.0);
}
