#ifndef LOGONMAIN_H
#define LOGONMAIN_H

#include"touch.h"
#include"clinet.h"
#include"bmp.h"
#include "mfont.h"
class LongonMain
{
private:
    char m_Mainfilename[12];
    Touch touch;
    Bmp bmp;
    pthread_t tid_t1;
public:
    LongonMain();
    ~LongonMain();
    void connectThread();
    void run(Clinet* client = Clinet::instance(),MFont* font = MFont::instance());
    void getPhnoe(char *ph_buf);
    void getPasswd(char *pd_buf);
    const char *keyInput();


};

#endif // LOGONMAIN_H
