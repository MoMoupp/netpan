#include "sever.h"


extern list<userdata*> UserdataList;

void InitUL(cchp_t datafilename)
{
    FILE *fp = fopen(datafilename,"r");
    if(fp==NULL){
        throw "datafile not find";
    }
    while(1){
        char buf[120];
        if (fgets(buf, 200, fp) == NULL)
        {
            perror("fgts:");
            break;
        }
        userdata* data_ptr = new userdata;
        sscanf(buf,"%[^@]@%[^@]@%d[^/n]",data_ptr->phone,data_ptr->passwd,&(data_ptr->isVIp));
        data_ptr->isOnline = false;
        cout<<data_ptr->phone<<"@"<<data_ptr->passwd<<endl;
        UserdataList.push_front(data_ptr);
    }
    fclose(fp);
}
sever::sever()
{
    InitUL(DATAFILE);
    MAddr.sin_family = AF_INET;
    MAddr.sin_port = htons(Sever_Port);
    MAddr.sin_addr.s_addr = inet_addr(Sever_IP);
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    this->BandL();
}

sever::~sever()
{
    close(socketFD);
}

void sever::BandL()
{
    int len_in = sizeof(struct sockaddr_in);
    int optval = 1;
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bind(socketFD, (struct sockaddr *)&MAddr, (socklen_t)len_in);
    listen(socketFD, 3);
}

void sever::Run()
{
    while (1)
    {
        std::cout << "等待新客户端连接。。。" << std::endl;
        struct sockaddr_in clientAddr;
        int len_a = sizeof(struct sockaddr_in);
        int CilentsocketFD;
        CilentsocketFD = accept(socketFD, (struct sockaddr *)&clientAddr, (socklen_t *)&len_a);
        if (CilentsocketFD != -1)
        {
            cout << "有客户端连接 ip:" << inet_ntoa(clientAddr.sin_addr)<< "port :"<< ntohs(clientAddr.sin_port) << endl;
            cchp_t ip = inet_ntoa(clientAddr.sin_addr);
            int port = ntohs(clientAddr.sin_port);
            //开启多线程管理客户端
            ClientManager *newclient = new ClientManager(ip,port,CilentsocketFD);
            thread threadClient(&ClientManager::run,newclient);
            threadClient.detach();
        }
        else
        {
            cout << "未知错误" << endl;
        }
    }

}
