#ifndef CLINETMANAGER_H
#define CLINETMANAGER_H

#include "client.h"

#define MAXSIEZ (1024)
#define DATAFILE "userdata.txt"
typedef struct userdata{
    char phone[12];
    char passwd[20];
    int isVIp;
    bool isOnline;
}userdata;

typedef struct senduserdata{
    char phone[12];
    char passwd[20];
}senduserdata;

typedef struct fileinfo_t
{
    char filename[256];
    long long size;
} fileinfo_t;

typedef struct FileInfo
{
    char filename[256];
    int type_t;
}FileInfo;


class ClientManager: public Client
{
private:
    string m_phone;
    string m_passwd;
    bool islogon;
    bool send_cancel_flag;
    list<userdata*>::iterator this_it;
public:
    ClientManager(cchp_t IP,int port,int socketfd);
    ~ClientManager();
    void run() override;

    int logon();
    int UserRegister();
    int logonout();

    int RecvFile();
    int ProcessDownloadRequests();
    void SendFile(cchp_t filename);
    void SendingCancel();

    int sendfileinfo(const char* dir);
};

#endif // CLINETMANAGER_H
