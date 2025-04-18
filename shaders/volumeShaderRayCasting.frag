#version 460

// Okay so with this shader we're effectively shooting rays through the volume cube's surface and accumulating density info

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
    const vec3 exit = texture(exitPoints, gl_FragCoord.xy / viewportSize).rgb;

    //fragColor = vec4(exit, 1.0f);
    //fragColor = vec4(entry, 1.0f);
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //return;

    const vec3 rayDir = normalize(exit - entry);
    float rayLength = length(exit - entry);
    vec3 rayPos = entry;

    float distanceTraveled = 0.0f;

    float densitySum = 0.0f;
    float alphaSum = 0.0f;

    vec4 colorSum = vec4(0.0f);

    vec3 rayStep = rayDir * sampleRate;
    while (distanceTraveled < rayLength)
    {
        float density = texture(volumeTexture, rayPos).r;
        vec4 rgba = texture(transferFunction, density);
        rgba.rgb *= rgba.a;

        // This makes sure that the sample contributes proportional to how much light is obscured
        colorSum += (1.0f - alphaSum) * rgba;
        alphaSum += (1.0f - alphaSum) * rgba.a;

        if (alphaSum > 0.96)
            break;

        distanceTraveled += sampleRate;
        rayPos = entry + rayDir * distanceTraveled;
    }

    fragColor = vec4(vec3(colorSum), 1.0f);
    //Right now, the alpha value actually doenst do anything since I havent enabled it in OpenGL. I don't know what would happen if I did
}
