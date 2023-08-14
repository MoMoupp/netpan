#include "client.h"

Client::Client(cchp_t IP, int port,int CilentsocketFD)
{
    m_ip = IP;
    m_port = port;
    m_CilentsocketFD = CilentsocketFD;
}

Client::~Client()
{
    close(m_CilentsocketFD);
}

void Client::run()
{
    while (1) {
        cout<<"old Client run"<<endl;
        sleep(1);
    }

}

int Client::Recv(void *msgbuf, int size_t, bool f)
{
    if(f){
        cout<<"Wait["<<m_ip<<"]"<<":["<<m_port<<"] data"<<endl;
    }
    bzero(msgbuf,size_t);
    return recv(m_CilentsocketFD,msgbuf,size_t,0);
}

int Client::Send(void *msgbuf, int size_t)
{
    return send(m_CilentsocketFD,msgbuf,size_t,MSG_NOSIGNAL);
}

int Client::Send(const char *msgbuf, int size_t)
{
    return send(m_CilentsocketFD,msgbuf,size_t,MSG_NOSIGNAL);
}
