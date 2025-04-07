#pragma once

#include <GL/glew.h>
#include <vector>

GLuint makeGrayscaleTransferFunction()
{
    constexpr int size{ 256 };
    std::vector<GLubyte> data(size * 4);
    for (int i{ 0 }; i < size; ++i)
    {
        const float density{ i / static_cast<float>(size - 1) };

        const GLubyte densityAsByte( static_cast<GLubyte>(density * 255.0f) );
        data[4 * i] = densityAsByte;
        data[4 * i + 1] = densityAsByte;
        data[4 * i + 2] = densityAsByte;
        data[4 * i + 3] = densityAsByte;

    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_1D, textureID);
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    return textureID;
}

GLuint makeColorTransferFunction()
{
    constexpr int size{ 256 };
    std::vector<GLubyte> data(size * 4);

    for (int i { 0 }; i < size; ++i)
    {
        const float density{ i / static_cast<float>(size - 1) };

        // Start out black
        GLubyte red{ 0 }, green{ 0 }, blue{ 0 }, alpha{ 0 };

        if (density < 0.33f) // Red
        {
            const float intensity{ density / 0.33f };
            red = static_cast<GLubyte>((density / 0.33f) * 255.0f);
            alpha = static_cast<GLubyte>(intensity * 0.5f * 255.0f);
        }
        else if (density < 0.66f) // Red + green
        {
            // Add yellow
            const float intensity{ (density - 0.33f) / 0.33f };
            red = 255;
            green = static_cast<GLubyte>(intensity * 255.0f);
            alpha = static_cast<GLubyte>((0.5f + intensity * 0.3f) * 255.0f);
        }
        else // Red + green + blue
        {
            const float intensity{ (density - 0.66f) / 0.33f };
            red = green = 255;
            blue = static_cast<GLubyte>(intensity * 255.0f);
            alpha = static_cast<GLubyte>((0.8f + intensity * 0.2f) * 255.0f);
        }

        data[4 * i] = red;
        data[4 * i + 1] = green;
        data[4 * i + 2] = blue;
        data[4 * i + 3] = alpha;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_1D, textureID);
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    return textureID;
}
