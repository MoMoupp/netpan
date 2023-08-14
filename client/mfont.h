#ifndef MFONT_H
#define MFONT_H

extern "C"{
    #include "font.h"
}

#include "lcd.h"

class MFont
{
    MFont(unsigned int*lcd_p = Lcd::instance()->addr());
public:
    enum Color{
        GREEN = 0x0000ff00,
        RED = 0x00ff0000,
        WHITE = 0x00ffffff,
        YELLOW = 0x00ffff00,
        MCOLOR = 0x93606782
    };
    MFont(const MFont&other)=delete;
    ~MFont();

    static MFont* instance();

    static int show(char *text, int pixelSize = 24, int bgWidth = 100, int bgHeight = 100, u32 bgColor = WHITE, int posX = 0, int posY = 0,int fontPosX = 0,int fontPosY = 0,u32 fontColor = YELLOW);
    static int show(const char *text, int pixelSize = 24, int bgWidth = 100, int bgHeight = 100, u32 bgColor = WHITE, int posX = 0, int posY = 0,int fontPosX = 0,int fontPosY = 0,u32 fontColor = GREEN);
protected:
    void init();
    int destory();
private:
    static font *f;
    static unsigned int *lcd_p;

    static MFont *m_instance;
};


class UMFont
{

public:
    enum Color{
        GREEN = 0x0000ff00,
        RED = 0x00ff0000,
        WHITE = 0x00ffffff,
        YELLOW = 0x00ffff00,
        MCOLOR = 0x93606782
    };
    UMFont(unsigned int*lcd_p = Lcd::instance()->addr());
    ~UMFont();
     int show(char *text, int pixelSize = 24, int bgWidth = 100, int bgHeight = 100, u32 bgColor = WHITE, int posX = 0, int posY = 0,int fontPosX = 0,int fontPosY = 0,u32 fontColor = YELLOW);
     int show(const char *text, int pixelSize = 24, int bgWidth = 100, int bgHeight = 100, u32 bgColor = WHITE, int posX = 0, int posY = 0,int fontPosX = 0,int fontPosY = 0,u32 fontColor = GREEN);
protected:
    void init();
    int destory();
private:
    font *f;
    unsigned int *lcd_p;

};
#endif // MFONT_H
