#version 460 core

out vec3 fragColor;

in vec3 texCoord;

void main()
{
    fragColor = texCoord;
}
