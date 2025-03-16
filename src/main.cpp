#include <GL/freeglut.h>
#include <iostream>
#include "Input.h"
#include "DicomHandler.h"

void resizeWindow(int width, int height);
void renderScene();
void update();

const std::vector<GLfloat> quadVertices
{
    -1.0f, -1.0f, 0.0f, // Bottom left
    1.0f, -1.0f, 0.0f,  // Bottom right
    -1.0f, 1.0f, 0.0f,  // Top left
    1.0f, 1.0f, 0.0f,   // Top right
};
DicomHandler dicomImage;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    const int screenWidth{ glutGet(GLUT_SCREEN_WIDTH) };
    const int screenHeight{ glutGet(GLUT_SCREEN_HEIGHT) };
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Volume rendering");
    glutFullScreen();

    // Handle DICOM
    dicomImage.loadDICOM("../assets/series-000001/image-000182.dcm");

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
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dicomImage.getTextureID());

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

void resizeWindow(int width, int height)
{
    glViewport(0, 0, width, height);
}

