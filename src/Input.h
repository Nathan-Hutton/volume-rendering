#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Input
{
    int lastMouseYForDistance{};
    bool rightMouseHeld{ false };
    GLfloat cameraDistanceChange{};
    GLfloat viewDistance{ -5.0f };

    bool leftMouseHeld{ false };
    int lastXCursorPosForRotation{};
    int lastYCursorPosForRotation{};
    GLfloat xCursorPosChangeForRotation;
    GLfloat yCursorPosChangeForRotation;
    GLfloat xCameraRotateAmount;
    GLfloat yCameraRotateAmount;

    void keyboardInputCallback(unsigned char key, [[maybe_unused]] int x, [[maybe_unused]] int y)
    {
        if (key == 27)
            glutLeaveMainLoop();
    }

    void mouseButtonCallback(int button, int state, [[maybe_unused]] int x, [[maybe_unused]] int y)
    {
        if (button == GLUT_RIGHT_BUTTON)
        {
            if (state == GLUT_UP)
            {
                rightMouseHeld = false;
                cameraDistanceChange = 0.0f;
            }
            else
            {
                rightMouseHeld = true;
                lastMouseYForDistance = y;
            }
        }

        if (button == GLUT_LEFT_BUTTON)
        {
            if (state == GLUT_DOWN)
            {
                leftMouseHeld = true;
                lastXCursorPosForRotation = x;
                lastYCursorPosForRotation = y;
            }
            else
            {
                leftMouseHeld = false;
                xCursorPosChangeForRotation = 0.0f;
                yCursorPosChangeForRotation = 0.0f;
            }
        }
    }

    void mouseDragCallback(int x, int y)
    {
        if (rightMouseHeld)
        {
            cameraDistanceChange = static_cast<float>(lastMouseYForDistance - y);
            lastMouseYForDistance = y;
        }

        if (leftMouseHeld)
        {
            xCursorPosChangeForRotation = static_cast<float>(lastXCursorPosForRotation - x);
            yCursorPosChangeForRotation = static_cast<float>(lastYCursorPosForRotation - y);
            xCameraRotateAmount += xCursorPosChangeForRotation;
            yCameraRotateAmount += yCursorPosChangeForRotation;
        }
    }
}
