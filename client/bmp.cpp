#include "bmp.h"


Bmp::Bmp(const char *bmpName, unsigned int *lcd_p)
{
    memset(m_bmpName,0,sizeof(m_bmpName));
    strcpy(m_bmpName,bmpName);

    this->lcd_p = lcd_p;

    //获取图片的属性  --width  height  size
    int bmpFd = open(m_bmpName,O_RDONLY);
    if(bmpFd == -1)
    {
        printf("%s open bmp error:%s\n",m_bmpName,strerror(errno));
    }

    BITMAPFILEHEADER fileHeader;
    read(bmpFd,&fileHeader,14);
    BITMAPINFOHEADER infoHeader;
    read(bmpFd,&infoHeader,40);

    m_width = infoHeader.biWidth;
    m_height = infoHeader.biHeight;
    m_size = infoHeader.biSizeImage+54;

    close(bmpFd);
}

Bmp::Bmp(unsigned int *lcd_p)
{
    memset(m_bmpName,0,sizeof(m_bmpName));

    this->lcd_p = lcd_p;

    m_width = 0;
    m_height = 0;
    m_size = 0;
}

int Bmp::show(int posX,int posY)
{
    //2、打开图片
    int bmpFd = open(m_bmpName,O_RDONLY);
    if(bmpFd == -1)
    {
        printf("%s open bmp error:%s\n",m_bmpName,strerror(errno));
        return -1;
    }
    //偏移文件光标位置到  第54个字节开始 ，因为从这里开始，才是图形的颜色数据
    //文件头+信息头 ---》关于这个bmp文件的信息描述
    BITMAPFILEHEADER fileHeader;
    read(bmpFd,&fileHeader,14);
    BITMAPINFOHEADER infoHeader;
    read(bmpFd,&infoHeader,40);

//    printf("图片的宽度:%d 高度:%d 大小:%d 位深:%d\n",infoHeader.biWidth,
//                                                   infoHeader.biHeight,
//                                                 infoHeader.biSizeImage+54,
//                                                infoHeader.biBitCount);

    m_width = infoHeader.biWidth;
    m_height = infoHeader.biHeight;
    // 计算填充的字节数
    int h_bit = (4 - ((m_width *3) % 4)) % 4;
    //3、读取图片的数据
    char bmpbuf[ m_width * m_height*3];
    int rd_num = 0; // 记录读取次数
    while (1)
    {
        // 一次读取一行
        size_t f = read(bmpFd, bmpbuf + (rd_num * m_width * 3), m_width*3);
        // 跳过填充的字节
        lseek(bmpFd, h_bit, SEEK_CUR);
        if (f == 0)
        {
            // perror("bmp_over");
            break;
        }
        rd_num++;
    }
    //4、将读取出来的图片数据的每个像素点 BGR ---转换成 ARGB
    int tempbuf[m_width*m_height];
    for(int i=0,j=0; i<= m_width*m_height; i++,j+=3)
    {
        tempbuf[i]  =  0x00<<24|bmpbuf[2+j]<<16 | bmpbuf[1+j] <<8 | bmpbuf[0+j] ;
    }
    //将图片的数据进行上下 180度颠倒
    int lcdbuf[m_width*m_height];
    for(int j=0; j<m_height; j++)
    {
        for(int i=0; i<m_width; i++)
        {
            lcdbuf[ m_width*j+ i] = tempbuf[(m_height-1-j)*m_width+i ];
        }
    }

    for(int j=0; j<m_height; j++)
    {
        for(int i=0; i<m_width; i++)
        {
            lcd_p[ posY*800+posX+800*j+ i] = lcdbuf[j*m_width+i];
        }
    }

    close(bmpFd);
    return 0;
}

int Bmp::show(int posX, int posY, int scale)
{
    //2、打开图片
    int bmpFd = open(m_bmpName,O_RDONLY);
    if(bmpFd == -1)
    {
        printf("%s open bmp error:%s\n",m_bmpName,strerror(errno));
        return -1;
    }
    //偏移文件光标位置到  第54个字节开始 ，因为从这里开始，才是图形的颜色数据
    //文件头+信息头 ---》关于这个bmp文件的信息描述
    BITMAPFILEHEADER fileHeader;
    read(bmpFd,&fileHeader,14);
    BITMAPINFOHEADER infoHeader;
    read(bmpFd,&infoHeader,40);

    printf("图片的宽度:%d 高度:%d 大小:%d 位深:%d\n",infoHeader.biWidth,
                                                   infoHeader.biHeight,
                                                 infoHeader.biSizeImage+54,
                                                infoHeader.biBitCount);

    //3、读取图片的数据
    // 计算填充的字节数
    int h_bit = (4 - ((m_width * 3) % 4)) % 4;
    //读取图片的数据
    char bmpbuf[m_width*m_height*3];
    int rd_num = 0; // 记录读取次数
        while (1)
        {
            // 一次读取一行
            size_t f = read(bmpFd, bmpbuf + (rd_num * m_width * 3), m_width*3);
            // 跳过填充的字节
            lseek(bmpFd, h_bit, SEEK_CUR);
            if (f == 0)
            {
                // perror("bmp_over");
                break;
            }
            rd_num++;
        }
    //4、将读取出来的图片数据的每个像素点 BGR ---转换成 ARGB
    int tempbuf[800*480]= {0};
    for(int i=0,j=0; i<800*480; i++,j+=3)
    {
        tempbuf[i]  =  0x00<<24|bmpbuf[2+j]<<16 | bmpbuf[1+j] <<8 | bmpbuf[0+j] ;
    }
    //将图片的数据进行上下 180度颠倒
    int lcdbuf[800*480] = {0};
    for(int j=0; j<480; j++)
    {
        for(int i=0; i<800; i++)
        {
            lcdbuf[ 800*j+ i] = tempbuf[(479-j)*800+i ];
        }
    }
    //将图片进行压缩16倍
    int minbuf[(800/scale)*(480/scale)];
    // int minbuf[200*120];
    for(int j=0; j<(480/scale); j++)
    {
        for(int i=0; i<(800/scale); i++)
        {
            minbuf[j*(800/scale)+i]  = lcdbuf [j*800*scale+i*scale];
        }
    }

    for(int j=0; j<(480/scale); j++)
    {
        for(int i=0; i<(800/scale); i++)
        {
            lcd_p[posY * 800 + posX + j*800+i ]  = minbuf[j*(800/scale)+i];
        }
    }

    close(bmpFd);

    return 0;
}

int Bmp::width()
{
    return m_width;
}

int Bmp::height()
{
    return m_height;
}

int Bmp::size()
{
    return m_size;
}

void Bmp::setFileName(const char *bmpName)
{
    memset(m_bmpName,0,sizeof(m_bmpName));
    strcpy(m_bmpName,bmpName);


    //获取图片的属性  --width  height  size
    int bmpFd = open(m_bmpName,O_RDONLY);
    if(bmpFd == -1)
    {
        printf("%s open bmp error:%s\n",m_bmpName,strerror(errno));
    }

    BITMAPFILEHEADER fileHeader;
    read(bmpFd,&fileHeader,14);
    BITMAPINFOHEADER infoHeader;
    read(bmpFd,&infoHeader,40);

    m_width = infoHeader.biWidth;
    m_height = infoHeader.biHeight;
    m_size = infoHeader.biSizeImage+54;

    close(bmpFd);
}

char *Bmp::fileName()
{
    return m_bmpName;
}
