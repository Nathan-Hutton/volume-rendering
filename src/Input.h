#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>

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
            viewDistance += static_cast<float>(lastMouseYForDistance - y) * 0.01f;
            viewDistance = std::clamp(viewDistance, -10.0f, -1.0f);
            lastMouseYForDistance = y;
        }

        if (leftMouseHeld)
        {
            xCameraRotateAmount -= static_cast<float>(lastYCursorPosForRotation - y) * 0.1f;
            yCameraRotateAmount -= static_cast<float>(lastXCursorPosForRotation - x) * 0.1f;
            xCameraRotateAmount = fmod(xCameraRotateAmount, 360.0f);
            yCameraRotateAmount = fmod(yCameraRotateAmount, 360.0f);
            lastXCursorPosForRotation = x;
            lastYCursorPosForRotation = y;
        }
    }
}
