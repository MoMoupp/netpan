#include "mfont.h"

MFont *MFont::m_instance= NULL;
font *MFont::f= NULL;
unsigned int *MFont::lcd_p= NULL;

MFont::MFont(unsigned int *lcd_p)
{
    this->lcd_p = lcd_p;

    init();
}

MFont::~MFont()
{
    destory();
}

MFont *MFont::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new MFont;
    }

    return m_instance;
}

void MFont::init()
{
    //打开字体
    f = fontLoad((char*)"/zayu/simfang.ttf");
}

int MFont::destory()
{
    //关闭字体
    fontUnload(f);
}
                                                                        //A R G B
int MFont::show(char *text, int pixelSize, int bgWidth, int bgHeight, u32 bgColor, int posX, int posY,int fontPosX ,int fontPosY ,u32 fontColor )
{
    if(f == NULL || lcd_p == NULL)
    {
        printf("show error:f is NULL or lcd_p is NULL\n");
        return -1;
    }
    //1、字体大小的设置
    fontSetSize(f,pixelSize);

    //2、创建一个画板（点阵图）
    //int  bgColor = 0x00ffff00; //如何将里面的rgb 切割出来

    //getColor(a, b, c, d) (a|b<<8|c<<16|d<<24)  A B G R
    //注意：颜色的设置要用  getColor(A,B,G,R)
    int A = bgColor >>24 &0xff;
    int R = bgColor >>16 & 0xff;
    int G = bgColor >>8 &0xff;
    int B = bgColor &0xff;
    bitmap *bm = createBitmapWithInit(bgWidth,bgHeight,4,getColor(A, B, G, R)); //也可使用createBitmapWithInit函数，改变画板颜色

    //3、将字体写到点阵图上(写到画板)
    int a = fontColor >>24 &0xff;
    int r = fontColor >>16 & 0xff;
    int g = fontColor >>8 &0xff;
    int b = fontColor &0xff;
    fontPrint(f,bm,fontPosX,fontPosY,(char*)text,getColor(a, b, g, r),0);

    //4、把画板显示到LCD屏幕上
    show_font_to_lcd(lcd_p,posX,posY,bm);

    //5、销毁画板
    destroyBitmap(bm);
}
int MFont::show(const char *text,int pixelSize, int bgWidth, int bgHeight, u32 bgColor, int posX, int posY,int fontPosX ,int fontPosY ,u32 fontColor )
{
    show((char*)text,  pixelSize,  bgWidth,  bgHeight,  bgColor,  posX,  posY, fontPosX , fontPosY , fontColor );
}


UMFont::UMFont(unsigned int *lcd_p)
{
    this->lcd_p = lcd_p;

    init();
}

UMFont::~UMFont()
{
    destory();
}



void UMFont::init()
{
    //打开字体
    f = fontLoad((char*)"/zayu/simfang.ttf");
}

int UMFont::destory()
{
    //关闭字体
    fontUnload(f);
}
                                                                        //A R G B
int UMFont::show(char *text, int pixelSize, int bgWidth, int bgHeight, u32 bgColor, int posX, int posY,int fontPosX ,int fontPosY ,u32 fontColor )
{
    if(f == NULL || lcd_p == NULL)
    {
        printf("show error:f is NULL or lcd_p is NULL\n");
        return -1;
    }
    //1、字体大小的设置
    fontSetSize(f,pixelSize);

    //2、创建一个画板（点阵图）
    //int  bgColor = 0x00ffff00; //如何将里面的rgb 切割出来

    //getColor(a, b, c, d) (a|b<<8|c<<16|d<<24)  A B G R
    //注意：颜色的设置要用  getColor(A,B,G,R)
    int A = bgColor >>24 &0xff;
    int R = bgColor >>16 & 0xff;
    int G = bgColor >>8 &0xff;
    int B = bgColor &0xff;
    bitmap *bm = createBitmapWithInit(bgWidth,bgHeight,4,getColor(A, B, G, R)); //也可使用createBitmapWithInit函数，改变画板颜色

    //3、将字体写到点阵图上(写到画板)
    int a = fontColor >>24 &0xff;
    int r = fontColor >>16 & 0xff;
    int g = fontColor >>8 &0xff;
    int b = fontColor &0xff;
    fontPrint(f,bm,fontPosX,fontPosY,(char*)text,getColor(a, b, g, r),0);

    //4、把画板显示到LCD屏幕上
    show_font_to_lcd(lcd_p,posX,posY,bm);

    //5、销毁画板
    destroyBitmap(bm);
}
int UMFont::show(const char *text,int pixelSize, int bgWidth, int bgHeight, u32 bgColor, int posX, int posY,int fontPosX ,int fontPosY ,u32 fontColor )
{
    show((char*)text,  pixelSize,  bgWidth,  bgHeight,  bgColor,  posX,  posY, fontPosX , fontPosY , fontColor );
}
