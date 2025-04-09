#version 460 core

out vec3 fragColor;

in vec3 texCoord;

void main()
{
    fragColor = vec3(1.0f, 0.0f, 0.0f);
    fragColor = texCoord;
}
