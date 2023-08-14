#ifndef PANMAIN_H
#define PANMAIN_H

#include"touch.h"
#include"clinet.h"
#include"bmp.h"
#include "mfont.h"
#include <dirent.h>
#include <queue>
typedef struct FileInfo
{
    char filename[256];
    int type_t;
}FileInfo;

typedef struct tasklist{
    char path[256];
    char filename[256];
    int TaskType;
}tasklist;

enum TType{
    Download,
    Upload
};

class PanMain
{

private:
    pthread_t m_tasktid;
    pthread_t m_stasktid;
    pthread_t m_swtid;
    int m_dircount;
    int m_pandircount;
    int n_pages;
    int max_pages;
    int n_Ppages;
    int max_Ppages;
    int listnum;
    char m_nowpath[256];
    bool changepath;
public:
    PanMain();
    ~PanMain();
    bool run();

    void Recvfileinfo(Clinet* clinet= Clinet::instance());
    void GetLocalFileInfo(const char* path);
    void showlist();
    void ShowLocalList();
    void cleanlist(int n);

    void taskthread();
    void listThread();
    void showWaitThread();
};

#endif // PANMAIN_H
