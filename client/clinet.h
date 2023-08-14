#ifndef CLINET_H
#define CLINET_H
#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <iomanip>
using namespace std;
#define Sever_IP "47.99.174.43"
#define Sever_Port 50002
#define MAXSIZE (1024)

typedef struct senduserdata{
    char phone[12];
    char passwd[20];
}senduserdata;

class Clinet
{
private:
    struct sockaddr_in severAddr;
    int socketFD;

    string m_phone;
    static Clinet *m_instance;
    static bool m_severOnline;
    static bool islogon;
    Clinet();

public:
    Clinet(const Clinet&other)=delete;
    ~Clinet();
    static Clinet* instance();
    static bool severOnline();
    static bool Islogon();
    static void SetIslogon(bool flag);
    void Connect();
    int Sendmsg(void*buf,int size_t);
    int Sendmsg(const char*buf,int size_t);
    int Recvmsg(void*buf,int size_t);

    void Sendfile(const char *filename, const char*path);
    void Recvfile(const char *filename,const char* path);
    void ClearRecvbuf(char*buf);
    void logn(const char* phone,const char* passwd);
    void UserRegister(const char* phone,const char* passwd);
};

#endif // CLINET_H
