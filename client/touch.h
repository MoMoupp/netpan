#ifndef TOUCH_H
#define TOUCH_H

#include <cstring>
#include <errno.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 #include <unistd.h>
#include <iostream>

#include <linux/input.h>

#include "point.h"

class Touch
{
public:
    enum Board{
        Board_GEC6818_BLACK, //黑色面板
        Board_GEC6818_BLUE,//蓝色面板
    };

    Touch(enum Board type = Board_GEC6818_BLACK, const char*deviceName = "/dev/input/event0");

    int wait(int &x,int &y);
    Point wait();
    int wait(Point &p);

private:
    int m_x;
    int m_y;
    char m_deviceName[256];
    enum Board m_type;
};

#endif // TOUCH_H
