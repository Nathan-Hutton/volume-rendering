#pragma once

void processInput(unsigned char key, int x, int y)
{
    if (key == 27)
        glutLeaveMainLoop();
}

