#include "logonmain.h"

LongonMain::LongonMain()
{
    strcpy(m_Mainfilename,"longmain.bmp");
}

LongonMain::~LongonMain()
{
    pthread_cancel(tid_t1);

}

void LongonMain::connectThread()
{
    Clinet *client = Clinet::instance();
    UMFont font;
    while (1)
    {
        try {
            if(client->severOnline()==false){
                client->Connect();
            }
            if(client->severOnline())
            {
                font.show("服务器状态:在线",24,200,24,MFont::WHITE,245,456,0,0,0x00);
                break;
            }
        } catch (const char * str) {
            font.show(str,24,200,24,MFont::WHITE,245,456,0,0,0x00);
        }
        usleep(20);
    }

}

void LongonMain::run(Clinet *client,MFont* font)
{

    bmp.setFileName(m_Mainfilename);
    bmp.show();

    thread t1(&LongonMain::connectThread,this);
    tid_t1 = t1.native_handle();
    t1.detach();


    int x,y;
    char ph_buf[128]={0};
    char pd_buf[128]={0};
    while (1) {
        touch.wait(x,y);
        //退出
        if(x>730&&y>0 && x<800&&y<70){
            bmp.setFileName("black.bmp");
            bmp.show();
            exit(0);
        }
        //点到输入框1
        if(x>293&&y>107 && x<757&&y<161 )
        {
            getPhnoe(ph_buf);
        }
        //点到输入框2
        else if(x>293&&y>182 && x<757&&y<242 ){
            getPasswd(pd_buf);
        }
        //登录
        else if(x>301&&y>295 && x<752&&y<355 ){

            if(strlen(ph_buf)!=11)
            {
                font->show("请输入11位手机号",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
            if(!client->severOnline()){
                font->show("未连接到服务器",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
            try {
                client->logn(ph_buf,pd_buf);
                font->show("登录成功,请稍后",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                return;
            } catch (const char* str) {
                font->show(str,24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
        }
        //注册
        else if(x>484&&y>386 && x<585&&y<420){

            if(strlen(ph_buf)!=11)
            {
                font->show("请输入11位手机号",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
            int len = strlen(pd_buf);
            if(len==0){
                font->show("密码不能为空",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
            else if(len>=20){
                font->show("密码过长",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }

            if(!client->severOnline()){
                font->show("未连接到服务器",24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
            try {
                client->UserRegister(ph_buf,pd_buf);
                font->show("注册成功,请稍后",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                return;
            } catch (const char* str) {
                font->show(str,24,300,24,MFont::WHITE,245,252,0,0,0x00);

                sleep(2);
                font->show("",24,300,24,MFont::WHITE,245,252,0,0,0x00);
                continue;
            }
        }
    }

}

void LongonMain::getPhnoe(char *ph_buf)
{
    bmp.setFileName("keyboard.bmp");
    bmp.show(600,280);

    MFont::instance()->show(ph_buf,45,364,50,MFont::WHITE,387,108,0,5,0x00);

    while (1)
    {
        char key[2];
        int num = strlen(ph_buf);
        strcpy(key, this->keyInput());
        if (strcmp(key,"C")==0)
        {
            ph_buf[0] = '\0';

        }
        else if (strcmp(key, "D") == 0)
        {
            if (num==0)
            {
                continue;
            }
            ph_buf[num - 1] = '\0';
        }
        else if (strcmp(key, "T") == 0 || strcmp(key, "Q") == 0)
        {
            bmp.setFileName("hide_keyborad.bmp");
            bmp.show(600,280);
            return;
        }
        else if (strcmp(key, ".") == 0 || strcmp(key, "N") == 0)
        {
            continue;
        }
        else
        {
            strcat(ph_buf, key);
        }

        MFont::instance()->show(ph_buf,45,364,50,MFont::WHITE,387,108,0,5,0x00);

    }
}

void LongonMain::getPasswd(char *pd_buf)
{
    bmp.setFileName("keyboard.bmp");
    bmp.show(600,280);
    char hide[128]={0};

    unsigned int i;
    for(i =0;i<strlen(pd_buf);i++){
        hide[i] = '*';
    }
    hide[i] = '\0';
    MFont::instance()->show(hide,45,386,50,MFont::WHITE,367,186,0,5,0x00);

    while (1)
    {
        char key[2];
        int num = strlen(pd_buf);
        strcpy(key, this->keyInput());
        if (strcmp(key,"C")==0)
        {
            pd_buf[0] = '\0';

        }
        else if (strcmp(key, "D") == 0)
        {
            if (num==0)
            {
                continue;
            }
            pd_buf[num - 1] = '\0';
        }
        else if (strcmp(key, "T") == 0 || strcmp(key, "Q") == 0)
        {
            bmp.setFileName("hide_keyborad.bmp");
            bmp.show(600,280);
            return;
        }
        else if (strcmp(key, ".") == 0 || strcmp(key, "N") == 0)
        {
            continue;
        }
        else
        {
            strcat(pd_buf, key);
        }

        for(i =0;i<strlen(pd_buf);i++){
            hide[i] = '*';
        }
        hide[i] = '\0';
        MFont::instance()->show(hide,45,386,50,MFont::WHITE,367,186,0,5,0x00);
    }
}

const char *LongonMain::keyInput(){
        int x, y;
        touch.wait(x, y);
        if (x > 620 && y > 300 && x < 650 && y < 330)
        {
            return "1";
        }
        else if (y > 300 && y < 330 && x > 660 && x < 690)
        {
            return "2";
        }
        else if (y > 300 && y < 330 && x > 700 && x < 730)
        {
            return "3";
        }
        else if (y > 343 && y < 375 && x > 620 && x < 650)
        {
            return "4";
        }
        else if (y > 343 && y < 375 && x > 660 && x < 690)
        {
            return "5";
        }
        else if (y > 343 && y < 375 && x > 700 && x < 730)
        {
            return "6";
        }
        else if (y > 385 && y < 416 && x > 620 && x < 650)
        {
            return "7";
        }
        else if (y > 385 && y < 416 && x > 660 && x < 690)
        {
            return "8";
        }
        else if (y > 385 && y < 416 && x > 700 && x < 730)
        {
            return "9";
        }
        else if (y > 426 && y < 475 && x > 620 && x < 650)
        {
            return "0";
        }
        else if (y > 343 && y < 375 && x > 740 && x < 800)
        {
            return "D";
        }
        else if (y > 385 && y < 416 && x > 740 && x < 800)
        {
            return "C";
        }
        else if (y > 426 && y < 475 && x > 700 && x < 800)
        {
            return "T";
        }
        else if (y > 426 && y < 475 && x > 660 && x < 700)
        {
            return ".";
        }
        else if (y > 300 && y < 330 && x > 740 && x < 785)
        {
            return "Q";
        }
        else
        {
            return "Q";
        }
}
