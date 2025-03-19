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

void main()
{
    vec3 rayPos = texCoord;

    float densitySum = 0.0f;
    //float alphaSum = 0.0f;

    const int numSamples = 100;
    for (int i = 0; i < numSamples; ++i)
    {
        float sampleDensity = texture(volumeTexture, rayPos).r;
        densitySum += sampleDensity * (1.0f / numSamples);
        //alphaSum += sampleDensity * 0.01f;

        rayPos += rayDirection * sampleRate;

        if (rayPos.x < 0.0f || rayPos.x > 1.0f || rayPos.y < 0.0f || rayPos.y > 1.0f || rayPos.z < 0.0f || rayPos.z > 1.0f)
            break;
    }

    fragColor = vec4(densitySum, densitySum, densitySum, 1.0f);
    //fragColor = vec4(densitySum, densitySum, densitySum, alphaSum);
    // Right now, the alpha value actually doens't do anything since I haven't enabled it in OpenGL. I don't know what would happen if I did
}
