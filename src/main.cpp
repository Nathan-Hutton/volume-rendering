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
    glutKeyboardFunc(Input::keyboardInputCallback);
    glutMouseFunc(Input::mouseButtonCallback);
    glutMotionFunc(Input::mouseDragCallback);

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
    glUniform1f(glGetUniformLocation(rayCastingShader, "sampleRate"), 0.01f);
    glUniform2f(glGetUniformLocation(rayCastingShader, "viewportSize"), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glutMainLoop();
    return 0;
}

void update() 
{
    //const glm::mat4 view{ 1.0f };
    glm::mat4 view{ glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, 0.0f, Input::viewDistance }) };
    view = glm::rotate(view, glm::radians(-Input::xCameraRotateAmount), glm::vec3{1.0f, 0.0f,0.0f});
    view = glm::rotate(view, glm::radians(-Input::yCameraRotateAmount), glm::vec3{0.0f, 1.0f, 0.0f});

    //view = glm::rotate(view, glm::radians(90.0f), glm::vec3{ 0.0f, 0.0f, 0.0f });
    const glm::mat4 projection{ glm::perspective(glm::radians(45.0f), static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)), 0.1f, 1000.0f) };
    //const float aspect{ static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)) };
    //const float orthoHeight{ 2.0f };
    //const float orthoWidth{ orthoHeight * aspect };
    //const glm::mat4 projection{ glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.1f, 1000.0f) };
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
    glFrontFace(GL_CCW);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(rayCastingShader);
    glUniformMatrix4fv(glGetUniformLocation(rayCastingShader, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    cube.draw();

    glutSwapBuffers();
}

void resizeWindow(int width, int height)
{
    glViewport(0, 0, width, height);
}

