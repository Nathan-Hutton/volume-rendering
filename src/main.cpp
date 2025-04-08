#include <iostream>
#include "Input.h"
#include "ShaderHandler.h"
#include "DicomHandler.h"
#include "Cube.h"
#include "ExitPointsBuffer.h"
#include "TransferFunction.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtc/type_ptr.hpp>

void resizeWindow(int width, int height);
void renderScene();
void update();

Cube cube;
ExitPointsBuffer exitPointsBuffer;
glm::mat4 mvp;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Volume rendering");
    glutFullScreen();

    const GLenum err{ glewInit() };
    if (err != GLEW_OK) {
        std::cerr << "GLEW Init Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glutDisplayFunc(renderScene);
    glutIdleFunc(update);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(processInput);

    compileShaders();

    // Handle DICOM
    DicomHandler dicomHandler;
    dicomHandler.loadDicomDirectory("../assets/lung-data");
    cube.init();

    // Setup all textures
    glUseProgram(rayCastingShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, dicomHandler.getTextureID());
    glUniform1i(glGetUniformLocation(rayCastingShader, "volumeTexture"), 0);
    
    const GLuint transferFunction{ makeColorTransferFunction() };
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, transferFunction);
    glUniform1i(glGetUniformLocation(rayCastingShader, "transferFunction"), 1);

    exitPointsBuffer.init();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, exitPointsBuffer.getTextureID());
    glUniform1i(glGetUniformLocation(rayCastingShader, "exitPoints"), 2);

    // Setup other uniform variables
    constexpr glm::vec3 rayDir{ 0.0f, 0.0f, -1.0f };
    glUniform3fv(glGetUniformLocation(rayCastingShader, "rayDirection"), 1, glm::value_ptr(rayDir));
    glUniform1f(glGetUniformLocation(rayCastingShader, "sampleRate"), 0.01f);

    glutMainLoop();
    return 0;
}

void update() 
{
    const glm::mat4 view{ 1.0f };
    constexpr glm::mat4 projection{ 1.0f };
    mvp = projection * view; // We just won't do a model transform

    glutPostRedisplay();
}

void renderScene()
{
    // Record exit points of the cube
    glCullFace(GL_FRONT);
    exitPointsBuffer.bindForWriting();
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(exitCoordsShader);
    glUniformMatrix4fv(glGetUniformLocation(exitCoordsShader, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    cube.draw();

    // Render to the main buffer
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(rayCastingShader);
    glUniformMatrix4fv(glGetUniformLocation(rayCastingShader, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(glGetUniformLocation(rayCastingShader, "exitPoints"), 0);
    cube.draw();

    glutSwapBuffers();
}

void resizeWindow(int width, int height)
{
    glViewport(0, 0, width, height);
}

