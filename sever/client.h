#ifndef CLIENT_H
#define CLIENT_H
#define ABCD
#include <iostream>
#include <list>
#include <cstdio>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>  /* See NOTES */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
using namespace  std;
typedef  const char*  cchp_t;

class Client
{
protected:
    string m_ip;
    int m_port;
    int m_CilentsocketFD;
public:
    Client(cchp_t IP,int port,int CilentsocketFD);
    virtual ~Client();
    virtual void run();
    int Recv(void*msgbuf,int size_t,bool f=true);
    int Send(void*msgbuf,int size_t);
    int Send(const char*msgbuf,int size_t);
};




#endif // CLIENT_H
