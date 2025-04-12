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
#include <GL/glui.h>

void resizeWindow(int width, int height);
void renderScene();
void update();

Cube cube;
ExitPointsBuffer exitPointsBuffer;
glm::mat4 mvp;

int mainWindowID{ 0 };
int gluiWindowID{ 0 };

TransferFunction transferFunction;

void idleCallback()
{
    glutSetWindow(mainWindowID);
    glutPopWindow();
    glutSetWindow(gluiWindowID);
    glutPopWindow();
    glutSetWindow(mainWindowID);
    update();
}

void quitApp(int)
{
    exit(0);
}

void gluiSpinnerCallback([[maybe_unused]] int controlID)
{
    transferFunction.resetData();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Volume rendering");
    glutFullScreen();
    mainWindowID = glutGetWindow();

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
    
    //transferFunction.setAsColorTransferFunction();
    transferFunction.setAsGrayscaleTransferFunction();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, transferFunction.getTextureID());
    glUniform1i(glGetUniformLocation(rayCastingShader, "transferFunction"), 1);

    exitPointsBuffer.init();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, exitPointsBuffer.getTextureID());
    glUniform1i(glGetUniformLocation(rayCastingShader, "exitPoints"), 2);

    // GLUI
    GLUI* glui{ GLUI_Master.create_glui("Controls") };
    gluiWindowID = glui->get_glut_window_id();

    GLUI_Spinner* slider1{ glui->add_spinner("Opacity 1", GLUI_SPINNER_FLOAT, transferFunction.getOpacity1Pointer(), 1, gluiSpinnerCallback) };
    GLUI_Spinner* slider2{ glui->add_spinner("Opacity 2", GLUI_SPINNER_FLOAT, transferFunction.getOpacity2Pointer(), 1, gluiSpinnerCallback) };
    GLUI_Spinner* slider3{ glui->add_spinner("Opacity 3", GLUI_SPINNER_FLOAT, transferFunction.getOpacity3Pointer(), 1, gluiSpinnerCallback) };
    slider1->set_float_limits(0.0f, 1.0f);
    slider2->set_float_limits(0.0f, 1.0f);
    slider3->set_float_limits(0.0f, 1.0f);

    glui->add_checkbox("Color", transferFunction.getUsingColorPointer(), 1, gluiSpinnerCallback);

    glui->add_button("Quit", 0, quitApp);
    glui->set_main_gfx_window(glutGetWindow());
    GLUI_Master.set_glutIdleFunc(idleCallback);


    // Setup other uniform variables
    glUniform1f(glGetUniformLocation(rayCastingShader, "sampleRate"), 0.01f);
    glUniform2f(glGetUniformLocation(rayCastingShader, "viewportSize"), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glutMainLoop();
    return 0;
}

void update() 
{
    glm::mat4 view{ glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, 0.0f, Input::viewDistance }) };
    view = glm::rotate(view, glm::radians(Input::xCameraRotateAmount), glm::vec3{1.0f, 0.0f,0.0f});
    view = glm::rotate(view, glm::radians(Input::yCameraRotateAmount), glm::vec3{0.0f, 1.0f, 0.0f});

    const glm::mat4 projection{ glm::perspective(glm::radians(45.0f), static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)), 0.1f, 1000.0f) };
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

