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
uniform sampler2D exitPoints;
uniform float sampleRate;
uniform sampler1D transferFunction;
uniform vec2 viewportSize;

void main()
{
    const vec3 entry = texCoord;
    const vec3 exit = clamp(texture(exitPoints, gl_FragCoord.xy / viewportSize).rgb, 0.0, 1.0);
    //const vec3 exit = texture(exitPoints, gl_FragCoord.xy / viewportSize).rgb;

    fragColor = vec4(exit, 1.0f);
    //fragColor = vec4(entry, 1.0f);
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    return;

    if (distance(entry, exit) < 0.001)
    {   
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    const vec3 rayDir = normalize(exit - entry);
    float rayLength = length(exit - entry);
    vec3 rayPos = entry;

    //float t = 0.0f;

    float densitySum = 0.0f;
    float alphaSum = 0.0f;

    vec4 colorSum = vec4(0.0f);

    const int numSteps = int(rayLength / sampleRate);
    vec3 rayStep = rayDir * sampleRate;
    //while (t < rayLength)
    for (int i = 0; i < numSteps; ++i)
    {
        float density = texture(volumeTexture, rayPos).r;
        vec4 rgba = texture(transferFunction, density);
        rgba.rgb *= rgba.a;

        // This makes sure that the sample contributes proportional to how much light is obscured
        colorSum += (1.0f - alphaSum) * rgba;
        alphaSum += (1.0f - alphaSum) * rgba.a;

        if (alphaSum > 0.96)
            break;

        //t += sampleRate;
        rayPos += rayStep;
    }

    fragColor = vec4(vec3(colorSum), alphaSum);
    //Right now, the alpha value actually doenst do anything since I havent enabled it in OpenGL. I don't know what would happen if I did
}
