#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

class ExitPointsBuffer
{
    public:
        void init()
        {
            glGenFramebuffers(1, &m_framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "Something went wrong making the framebuffer for the exit points\n";

            const GLenum status{ glCheckFramebufferStatus(GL_FRAMEBUFFER) };
            if (status != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "Framebuffer incomplete! Status: " << std::hex << status << std::endl;

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void bindForWriting() const
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        }

        GLuint getTextureID() const
        {
            return m_textureID;
        }

    private:
        GLuint m_framebuffer;
        GLuint m_textureID;

};
