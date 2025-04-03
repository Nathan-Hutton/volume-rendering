#include <iostream>
#include "Input.h"
#include "ShaderHandler.h"
#include "DicomHandler.h"
#include "Cube.h"
#include "Quad.h"
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

//Quad quad;
Cube cube;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    const int screenWidth{ glutGet(GLUT_SCREEN_WIDTH) };
    const int screenHeight{ glutGet(GLUT_SCREEN_HEIGHT) };
    glutInitWindowSize(screenWidth, screenHeight);
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
    glCullFace(GL_BACK);

    compileShaders();

    // Handle DICOM
    DicomHandler dicomHandler;
    dicomHandler.loadDicomDirectory("../assets/lung-data");
    //quad.init();
    cube.init();

    glUseProgram(rayCastingShader);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, dicomHandler.getTextureID());
    glUniform1i(glGetUniformLocation(rayCastingShader, "volumeTexture"), 1);
    glUniform1f(glGetUniformLocation(rayCastingShader, "sliceIndex"), 92);

    constexpr glm::vec3 rayDir{ 0.0f, 0.0f, -1.0f };
    const GLuint transferFunction{ makeGrayscaleTransferFunction() };
    glUniform3fv(glGetUniformLocation(rayCastingShader, "rayDirection"), 1, glm::value_ptr(rayDir));
    glUniform1f(glGetUniformLocation(rayCastingShader, "sampleRate"), 0.01f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, transferFunction);
    glUniform1i(glGetUniformLocation(rayCastingShader, "transferFunction"), 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(renderScene);
    glutIdleFunc(update);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(processInput);

    glViewport(0, 0, screenWidth, screenHeight);

    glutMainLoop();

    return 0;
}

void update()
{
    // Put something here
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glUseProgram(volumeShader);
    glUseProgram(rayCastingShader);
    //quad.draw();
    cube.draw();

    glutSwapBuffers();
}

void resizeWindow(int width, int height)
{
    glViewport(0, 0, width, height);
}

