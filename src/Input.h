#pragma once

void processInput(unsigned char key, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
    if (key == 27)
        glutLeaveMainLoop();
}

