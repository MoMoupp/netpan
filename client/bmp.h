#ifndef BMP_H
#define BMP_H

#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>


#include "lcd.h"

class Bmp
{
    #define UINT  unsigned short
    #define DWORD unsigned  int
    #define LONG unsigned  int
    #define WORD unsigned short

    typedef struct tagBITMAPFILEHEADER
    {
        WORD bfType; // 位图文件的类型，必须为BM(1-2字节）
        DWORD bfSize; // 位图文件的大小，以字节为单位（3-6字节）
        WORD bfReserved1; // 位图文件保留字，必须为0(7-8字节）
        WORD bfReserved2; // 位图文件保留字，必须为0(9-10字节）
        DWORD bfOffBits; // 位图数据的起始位置，以相对于位图（11-14字节）
        // 文件头的偏移量表示，以字节为单位
    }BITMAPFILEHEADER;

    typedef struct tagBITMAPINFOHEADER{
        DWORD biSize; // 本结构所占用字节数（15-18字节）
        LONG biWidth; // 位图的宽度，以像素为单位（19-22字节）    ------图片的宽度
        LONG biHeight; // 位图的高度，以像素为单位（23-26字节）   ------图片的高度
        WORD biPlanes; // 目标设备的级别，必须为1(27-28字节）
        WORD biBitCount;// 每个像素所需的位数，必须是1（双色），（29-30字节）  ---位深
        // 4(16色），8(256色）或24（真彩色）之一
        DWORD biCompression; // 位图压缩类型，必须是 0（不压缩），（31-34字节）
        // 1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
        DWORD biSizeImage; // 位图的大小，以字节为单位（35-38字节）   ---图片颜色数据的大小
        LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数（39-42字节）
        LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数（43-46字节)
        DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数（47-50字节）
        DWORD biClrImportant;// 位图显示过程中重要的颜色数（51-54字节）
    }BITMAPINFOHEADER;


public:
    Bmp(const char*bmpName,unsigned int*lcd_p = Lcd::instance()->addr());
    Bmp(unsigned int*lcd_p = Lcd::instance()->addr());


    int show(int posX=0,int posY=0);
    int show(int posX,int posY,int scale); //将图片进行等比例缩小   切割

    int width();
    int height();
    int size();

    void setFileName(const char*bmpName);
    char*fileName();

private:
    char m_bmpName[256];
    int m_width;
    int m_height;
    int m_size;

    unsigned int *lcd_p;
};

#endif // BMP_H
