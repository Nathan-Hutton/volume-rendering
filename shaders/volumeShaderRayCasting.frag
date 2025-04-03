#version 460

// Okay so with this shader we're effectively shooting rays through the volume cube's surface and accumulating density info

// TODO:
// Right now, since I just use a fixed ray direction, this is basically orthographic. We'll want to make this work with our camera instead

// TODO:
// The rays also exit the cube based on the texture coordinates. This is a terrible idea since the rays won't always enter where the texture coords are 0.0f and whatnot.
// To fix this, we need to do multiple passes to store the entry and exit points

out vec4 fragColor;

in vec3 texCoord;

uniform sampler3D volumeTexture;
uniform vec3 rayDirection;
uniform float sampleRate;
uniform sampler1D transferFunction;

void main()
{
    vec3 rayPos = texCoord;

    float densitySum = 0.0f;
    float alphaSum = 0.0f;

    vec4 colorSum = vec4(0.0f);
    const int numSamples = 100;
    for (int i = 0; i < numSamples; ++i)
    {
        float sampleDensity = texture(volumeTexture, rayPos).r;
        vec4 rgba = texture(transferFunction, sampleDensity);
        rgba.rgb *= rgba.a;

        colorSum += (1.0f - alphaSum) * rgba;
        alphaSum += (1.0f - alphaSum) * rgba.a; // This makes sure that the sample contributes proportional to how much light is obscured

        if (alphaSum > 0.96)
            break;

        rayPos += rayDirection * sampleRate;
        if (rayPos.x < 0.0f || rayPos.x > 1.0f || rayPos.y < 0.0f || rayPos.y > 1.0f || rayPos.z < 0.0f || rayPos.z > 1.0f)
            break;
    }

    fragColor = vec4(vec3(colorSum), 1.0f);
    // Right now, the alpha value actually doens't do anything since I haven't enabled it in OpenGL. I don't know what would happen if I did
}
