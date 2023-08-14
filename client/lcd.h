#ifndef LCD_H
#define LCD_H

#include <cstring>
#include <errno.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 #include <unistd.h>
#include <iostream>

#include <sys/mman.h>

class Lcd
{
    Lcd(const char*deviceName = "/dev/fb0");
public:
    Lcd(const Lcd&other)=delete;
    ~Lcd();

    int init();

    //提供对外的接口
    unsigned int* addr();

    static Lcd* instance();
private:
    char m_deviceName[256];//液晶屏驱动文件的路径名
    int fd;
    unsigned int *lcd_p;

    static Lcd* m_instance;
};



#endif // LCD_H
