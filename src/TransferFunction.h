#pragma once

#include <GL/glew.h>
#include <vector>

class TransferFunction
{
    public:
        void setAsGrayscaleTransferFunction()
        {
            m_color = false;
            m_data.resize(m_size * 4);
            for (int i{ 0 }; i < m_size; ++i)
            {
                const float density{ i / static_cast<float>(m_size - 1) };

                const GLubyte densityAsByte( static_cast<GLubyte>(density * 255.0f) );
                m_data[4 * i] = densityAsByte;
                m_data[4 * i + 1] = densityAsByte;
                m_data[4 * i + 2] = densityAsByte;
                m_data[4 * i + 3] = densityAsByte;

            }

            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_1D, m_textureID);
            
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, m_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data());

            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        }

        void setAsColorTransferFunction()
        {
            m_color = true;
            m_data.resize(m_size * 4);
            for (int i { 0 }; i < m_size; ++i)
            {
                const float density{ i / static_cast<float>(m_size - 1) };

                // Start out black
                GLubyte red{ 0 }, green{ 0 }, blue{ 0 }, alpha{ 0 };

                if (density < 0.33f) // Red
                {
                    const float intensity{ density / 0.33f };
                    red = static_cast<GLubyte>((density / 0.33f) * 255.0f);
                    alpha = static_cast<GLubyte>(intensity * 0.5f * 255.0f * m_opacity1);
                }
                else if (density < 0.66f) // Red + green
                {
                    const float intensity{ (density - 0.33f) / 0.33f };
                    red = 255;
                    green = static_cast<GLubyte>(intensity * 255.0f);
                    alpha = static_cast<GLubyte>((0.5f + intensity * 0.3f) * 255.0f * m_opacity2);
                }
                else // Red + green + blue
                {
                    const float intensity{ (density - 0.66f) / 0.33f };
                    red = green = 255;
                    blue = static_cast<GLubyte>(intensity * 255.0f);
                    alpha = static_cast<GLubyte>((0.8f + intensity * 0.2f) * 255.0f * m_opacity3);
                }

                m_data[4 * i] = red;
                m_data[4 * i + 1] = green;
                m_data[4 * i + 2] = blue;
                m_data[4 * i + 3] = alpha;
            }

            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_1D, m_textureID);
            
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, m_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data());

            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        }

        void resetData()
        {

            if (m_color)
            {
                for (int i { 0 }; i < m_size; ++i)
                {
                    const float density{ i / static_cast<float>(m_size - 1) };

                    // Start out black
                    GLubyte red{ 0 }, green{ 0 }, blue{ 0 }, alpha{ 0 };

                    if (density < 0.33f) // Red
                    {
                        const float intensity{ density / 0.33f };
                        red = static_cast<GLubyte>((density / 0.33f) * 255.0f);
                        alpha = static_cast<GLubyte>(intensity * 0.5f * 255.0f * m_opacity1);
                    }
                    else if (density < 0.66f) // Red + green
                    {
                        const float intensity{ (density - 0.33f) / 0.33f };
                        red = 255;
                        green = static_cast<GLubyte>(intensity * 255.0f);
                        alpha = static_cast<GLubyte>((0.5f + intensity * 0.3f) * 255.0f * m_opacity2);
                    }
                    else // Red + green + blue
                    {
                        const float intensity{ (density - 0.66f) / 0.33f };
                        red = green = 255;
                        blue = static_cast<GLubyte>(intensity * 255.0f);
                        alpha = static_cast<GLubyte>((0.8f + intensity * 0.2f) * 255.0f * m_opacity3);
                    }

                    m_data[4 * i] = red;
                    m_data[4 * i + 1] = green;
                    m_data[4 * i + 2] = blue;
                    m_data[4 * i + 3] = alpha;
                }
            }
            else
            {
                for (int i{ 0 }; i < m_size; ++i)
                {
                    const float density{ i / static_cast<float>(m_size - 1) };

                    const GLubyte densityAsByte( static_cast<GLubyte>(density * 255.0f) );
                    m_data[4 * i] = densityAsByte;
                    m_data[4 * i + 1] = densityAsByte;
                    m_data[4 * i + 2] = densityAsByte;
                    m_data[4 * i + 3] = densityAsByte;

                }
            }

            glBindTexture(GL_TEXTURE_1D, m_textureID);
            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, m_size, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data());
        }

        GLuint getTextureID(){ return m_textureID; }

        float* getOpacity1Pointer(){ return &m_opacity1; }
        float* getOpacity2Pointer(){ return &m_opacity2; }
        float* getOpacity3Pointer(){ return &m_opacity3; }

    private:
        const int m_size{ 256 };
        std::vector<GLubyte> m_data;
        GLuint m_textureID;
        bool m_color;
        float m_opacity1{ 0.02f };
        float m_opacity2{ 1.0f };
        float m_opacity3{ 1.0f };
};
