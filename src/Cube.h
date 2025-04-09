#pragma once

#include <array>
#include <GL/glew.h>
#include <GL/freeglut.h>

class Cube
{
    public:
        Cube(){}

        void init()
        {
            const std::array<GLfloat, 48> cubeVertices
            {
                -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
                -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f
            };

            const std::array<GLuint, 36> cubeIndices = {
                0, 2, 1, 2, 3, 1,
                4, 5, 6, 6, 5, 7,
                0, 4, 2, 2, 4, 6,
                1, 3, 5, 3, 7, 5,
                0, 1, 4, 4, 1, 5,
                2, 6, 3, 6, 7, 3
            };

            GLuint cubeVBO, cubeEBO;
            glGenVertexArrays(1, &m_VAO);

            glBindVertexArray(m_VAO);

            glGenBuffers(1, &cubeVBO);
            glGenBuffers(1, &cubeEBO);

            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubeVertices.size(), cubeVertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cubeIndices.size(), cubeIndices.data(), GL_STATIC_DRAW);

            // Set vertex attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // Vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GL_FLOAT) * 3)); // Texture coordinates
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        void draw() const
        {
            glBindVertexArray(m_VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

    private:
        GLuint m_VAO;
};
