#include "lcd.h"

Lcd*Lcd::m_instance = NULL;

Lcd::Lcd(const char *deviceName)
{
    memset(m_deviceName,0,sizeof(m_deviceName));
    strcpy(m_deviceName,deviceName);

    init();
}

Lcd::~Lcd()
{
    munmap(lcd_p, 800*480*4);
    close(fd);
}

int Lcd::init()
{
    //打开液晶屏
    fd = open(m_deviceName,O_RDWR);
    if(fd == -1)
    {
        std::cout<< "open fb0 error:"<<strerror(errno)<<std::endl;
        return -1;
    }
    //内存映射
    lcd_p = (unsigned int*)mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(lcd_p == MAP_FAILED)
    {
        perror("mmap error");
        return -1;
    }
}

unsigned int *Lcd::addr()
{
    return lcd_p;
}

Lcd *Lcd::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new Lcd;
    }

    return m_instance;
}
