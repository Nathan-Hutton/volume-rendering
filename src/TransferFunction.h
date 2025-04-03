#pragma once

#include <GL/glew.h>
#include <vector>

GLuint makeGrayscaleTransferFunction()
{
    constexpr int size{ 256 };
    std::vector<GLubyte> data(size * 4);
    for (int i{ 0 }; i < size; ++i)
    {
        const float val{ i / static_cast<float>(size - 1) };

        const GLubyte valAsByte( static_cast<GLubyte>(val * 255.0f) );
        data[4 * i] = valAsByte;
        data[4 * i + 1] = valAsByte;
        data[4 * i + 2] = valAsByte;
        data[4 * i + 3] = valAsByte;

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
