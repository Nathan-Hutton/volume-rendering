#version 460

out vec4 fragColor;

in vec2 texCoord;

uniform sampler3D volumeTexture;
uniform float sliceIndex;

void main()
{
    vec3 volumeCoord = vec3(texCoord, sliceIndex);
	fragColor = texture(volumeTexture, volumeCoord);
}
