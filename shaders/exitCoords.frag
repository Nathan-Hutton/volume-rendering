#version 460 core

out vec4 fragColor;

in vec3 texCoord;

void main()
{
    fragColor = vec4(texCoord * 100.0f, 1.0f);
}
