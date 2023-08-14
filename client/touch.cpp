#include "touch.h"


Touch::Touch(enum Board type,const char *deviceName)
{
    strcpy(m_deviceName,deviceName);

    m_x = 0;
    m_y = 0;
    m_type = type;
}

int Touch::wait(int &x, int &y)
{
    //1、打开触摸屏文件
     int fd = open(m_deviceName,O_RDWR);
     if(fd == -1)
     {
         perror("open error");
         return -1;
     }
     //2、读取用手点击的触摸屏数据
     while(1)
     {
         struct input_event info;//存储触摸屏的数据
         read(fd,&info,sizeof(info));

         //判断当前产生的时候触摸屏事件 ,进一步判断是X轴事件
         if(info.type == EV_ABS && info.code == ABS_X)
         {
             x = info.value;
         }
         //判断当前产生的时候触摸屏事件 ,进一步判断是Y轴事件
         if(info.type == EV_ABS && info.code == ABS_Y)
         {
             y = info.value;
         }
         //松开的时候
         if(info.type ==EV_KEY  && info.code == BTN_TOUCH && info.value == 0)
         {
             if(m_type == Board_GEC6818_BLACK)
             {
                 //黑色屏幕坐标范围： X  0-1024  Y  0-600
                 //转换坐标范围
                 x = x * 800 *1.0/ 1024;
                 y = y * 480 *1.0/ 600;
                 //蓝色屏幕坐标范围： X  0-800  Y  0-480
             }

             printf("x:%d y:%d\n",x,y);
             break;
         }
     }

     //4、关闭触摸屏文件
     close(fd);
}

Point Touch::wait()
{
    int x,y;
    //1、打开触摸屏文件
     int fd = open(m_deviceName,O_RDWR);
     if(fd == -1)
     {
         perror("open error");
         return -1;
     }
     //2、读取用手点击的触摸屏数据
     while(1)
     {
         struct input_event info;//存储触摸屏的数据
         read(fd,&info,sizeof(info));

         //判断当前产生的时候触摸屏事件 ,进一步判断是X轴事件
         if(info.type == EV_ABS && info.code == ABS_X)
         {
             x = info.value;
         }
         //判断当前产生的时候触摸屏事件 ,进一步判断是Y轴事件
         if(info.type == EV_ABS && info.code == ABS_Y)
         {
             y = info.value;
         }
         //松开的时候
         if(info.type ==EV_KEY  && info.code == BTN_TOUCH && info.value == 0)
         {
             if(m_type == Board_GEC6818_BLACK)
             {
                 //黑色屏幕坐标范围： X  0-1024  Y  0-600
                 //转换坐标范围
                 x = x * 800 *1.0/ 1024;
                 y = y * 480 *1.0/ 600;
                 //蓝色屏幕坐标范围： X  0-800  Y  0-480
             }

             printf("x:%d y:%d\n",x,y);
             break;
         }
     }

     //4、关闭触摸屏文件
     close(fd);

     return Point(x,y);
}

int Touch::wait(Point &p)
{
    int x,y;
    //1、打开触摸屏文件
     int fd = open(m_deviceName,O_RDWR);
     if(fd == -1)
     {
         perror("open error");
         return -1;
     }
     //2、读取用手点击的触摸屏数据
     while(1)
     {
         struct input_event info;//存储触摸屏的数据
         read(fd,&info,sizeof(info));

         //判断当前产生的时候触摸屏事件 ,进一步判断是X轴事件
         if(info.type == EV_ABS && info.code == ABS_X)
         {
             x = info.value;
         }
         //判断当前产生的时候触摸屏事件 ,进一步判断是Y轴事件
         if(info.type == EV_ABS && info.code == ABS_Y)
         {
             y = info.value;
         }
         //松开的时候
         if(info.type ==EV_KEY  && info.code == BTN_TOUCH && info.value == 0)
         {
             if(m_type == Board_GEC6818_BLACK)
             {
                 //黑色屏幕坐标范围： X  0-1024  Y  0-600
                 //转换坐标范围
                 x = x * 800 *1.0/ 1024;
                 y = y * 480 *1.0/ 600;
                 //蓝色屏幕坐标范围： X  0-800  Y  0-480
             }

             printf("x:%d y:%d\n",x,y);
             break;
         }
     }

     //4、关闭触摸屏文件
     close(fd);

     p.setXY(x,y);

     return 0;
}
