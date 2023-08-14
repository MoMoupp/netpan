#include "clinet.h"
using namespace std;

Clinet* Clinet::m_instance = nullptr;
bool Clinet::m_severOnline = false;
bool Clinet::islogon = false;
bool cleanflag = false;
extern long long c_size;
extern long long t_size;
Clinet::Clinet()
{
    severAddr.sin_family = AF_INET;
    severAddr.sin_port = htons(Sever_Port);
    severAddr.sin_addr.s_addr = inet_addr(Sever_IP);
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    m_phone = "0";
}

Clinet::~Clinet()
{
    m_severOnline = false;
    islogon = false;
    close(socketFD);
}

Clinet* Clinet::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new Clinet;
    }
    return m_instance;
}

bool Clinet::severOnline()
{
    return m_severOnline;
}

bool Clinet::Islogon()
{
    return islogon;
}

void Clinet::SetIslogon(bool flag)
{
    islogon = flag;
}

void Clinet::Connect()
{
    int ret = connect(socketFD, (struct sockaddr *)&severAddr, sizeof(struct sockaddr_in));
    if (ret == 0)
    {
        m_severOnline = true;
        cout << "connet success" << endl;
    }
    else
    {
        throw "服务器状态: 离线";
    }
}

int Clinet::Sendmsg(void *buf, int size_t)
{
    return send(socketFD,buf,size_t,MSG_NOSIGNAL);
}

int Clinet::Sendmsg(const char *buf, int size_t)
{
    return send(socketFD,buf,size_t,MSG_NOSIGNAL);
}

int Clinet::Recvmsg(void *buf, int size_t)
{
    cout<<"Wait data"<<endl;
    bzero(buf,size_t);
    return recv(socketFD,buf,size_t,0);
}

void Clinet::Sendfile(const char *filename, const char*path)
{
    cleanflag = false;
    c_size = 0;
    t_size = 1;

    char msg[125] = {0};
    send(socketFD, "Upload request", 15, MSG_NOSIGNAL);

    cout << "Wait" << endl;
    int ret = this->Recvmsg(msg, 125);
    if(ret<=0){
        m_severOnline = false;
        throw "Sever disconnect";
    }
    if (strcmp(msg, "Accept") != 0)
    {
        throw "Sever Accept error";
    }

    char *openName = new char[512];
    sprintf(openName,"%s/%s",path,filename);
    int filefd = open(openName, O_RDONLY);
    if (filefd == -1)
    {
        delete [] openName;
        throw "No file";
    }
    delete [] openName;

    struct stat fileInfo;
    fstat(filefd, &fileInfo);
    struct fileinfo_t
    {
        char filename[256];
        long long size;
    } fileinfo_t;
    strcpy(fileinfo_t.filename, filename);
    fileinfo_t.size = fileInfo.st_size;
    send(socketFD, &fileinfo_t, sizeof(struct fileinfo_t), MSG_NOSIGNAL);
    t_size =  fileinfo_t.size;

    bzero(msg, 125);
    cout << "Wait" << endl;
    ret = this->Recvmsg(msg, 125);
    if(ret<=0){
        m_severOnline = false;
        throw "Sever disconnect";
    }
    if (strcmp(msg, "OK") != 0)
    {
        close(filefd);
        cout<<msg<<endl;
        throw "Sever system error";
    }

    //开始发送
    char *buf = new char[MAXSIZE];
    cout<<"开始上传"<<endl;
    cout << setiosflags(ios::fixed);// 只有在这项设置后，setprecision才是设置小数的位数。
    long long sendsize = 0;
    while (1)
    {
        //取消处理
        if (cleanflag)
        {
            sleep(2);
            send(socketFD, "cancel", 7, MSG_NOSIGNAL);
            return;
        }

        int ret = read(filefd, buf, MAXSIZE);
        if (ret == 0)
        {
            break;
        }

        int ret1 = send(socketFD, buf, ret, MSG_NOSIGNAL);
        if (ret1 <= 0)
        {
            m_severOnline = false;
            cout << "sever disconnect" << endl;
            break;
        }
        sendsize += ret1;
        c_size = sendsize;

    }
    cout << setprecision(2) << "send over:" << (sendsize * 1.0 / (MAXSIZE)) << "KB" << endl;
    c_size = t_size = 1;
    delete[] buf;
    close(filefd);

    cout << "Wait" << endl;
    bzero(msg, 125);
    ret = this->Recvmsg(msg, 125);
    if(ret<=0){
        m_severOnline = false;
        throw "Sever disconnect";
    }
    if (strcmp(msg, "Recv finsh") == 0)
    {
        cout << "send success" << endl;
        return;
    }

    cout << "send loss" << endl;
}

void Clinet::Recvfile(const char *filename,const char* path)
{
    cleanflag = false;
    c_size = 0;
    t_size = 1;
    char msg[125] = {0};
    send(socketFD, "Download request", 17, MSG_NOSIGNAL);

    cout << "Wait" << endl;
    int ret = this->Recvmsg(msg, 125);
    if(ret<=0){
        m_severOnline = false;
        throw "Sever disconnect";
    }
    if (strcmp(msg, "Accept") != 0)
    {
        throw "Sever Accept error";
    }
    send(socketFD, filename, strlen(filename) + 1, MSG_NOSIGNAL);

    cout << "Wait" << endl;
    bzero(msg, 125);

    ret = this->Recvmsg(msg, 125);
    if(ret<=0){
        m_severOnline = false;
        throw "Sever disconnect";
    }

    if (strcmp(msg, "has file") != 0)
    {
        throw "Sever system error or NO file";
    }

    long long size = 0;
    long long RecvSize = 0;
    recv(socketFD, &size, 8,0);
    t_size = size;
    cout << size << endl;

    char *openNamebat = new char[516];
    char *openName = new char[512];
    sprintf(openName,"%s/%s",path,filename);
    sprintf(openNamebat,"%s/%s.bat",path,filename);
    int filefd = open(openNamebat, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (filefd == -1)
    {
        perror("open:");
        send(socketFD, "Client system error", 20, MSG_NOSIGNAL);
        return;
    }

    send(socketFD, "OK", 3, MSG_NOSIGNAL);

    char *buf = new char[MAXSIZE];
    cout<<"开始接收"<<endl;
    cout << setiosflags(ios::fixed); // 只有在这项设置后，setprecision才是设置小数的位数。
    //int count = 0;
    while (1)
    {
        int rret = recv(socketFD, buf, MAXSIZE, 0);
        if (rret <= 0)
        {

            close(filefd);
            remove(openNamebat);
            delete[] buf;
            delete [] openName;
            delete [] openNamebat;
            m_severOnline = false;
            throw "服务器断开连接";
        }
        int wret = write(filefd, buf, rret);
        RecvSize += wret;
        c_size = RecvSize;
        //取消处理
        if(cleanflag){
            send(socketFD, "cancel", 7, MSG_NOSIGNAL);

            thread t1(&Clinet::ClearRecvbuf,this,buf);
            sleep(2);

            pthread_cancel(t1.native_handle());
            t1.join();

            close(filefd);
            remove(openNamebat);
            break;
        }


        if (RecvSize == size)
        {
            cout << "Download finsh: ";
            remove(openName);
            rename(openNamebat,openName);
            cout << setprecision(2) << RecvSize * 1.0 / (MAXSIZE) << "KB/" << size * 1.0  / (MAXSIZE) << "KB" << endl; // 输出2位小数
            break;
        }

    }
    delete[] buf;
    delete [] openName;
    delete [] openNamebat;
    close(filefd);
}

void Clinet::ClearRecvbuf(char*buf)
{
    while (1) {
        recv(socketFD,buf,MAXSIZE,0);
    }
}

void Clinet::logn(const char* phone, const char* passwd)
{
    char msg[125] = {0};
    send(socketFD, "Logon request", 14, MSG_NOSIGNAL);

    cout << "Wait" << endl;
    recv(socketFD, msg, 125, 0);
    if (strcmp(msg, "Accept") != 0)
    {
        cout<<msg<<endl;
        throw "Sever Accept error";
    }
    senduserdata data;
    strcpy(data.phone,phone);
    strncpy(data.passwd,passwd,20);
    data.passwd[19] = '\0';
    send(socketFD,&data,sizeof(senduserdata),MSG_NOSIGNAL);
    bzero(msg,125);
    int ret = recv(socketFD,msg,125,0);
    if(ret<=0){
        throw "服务器断开连接";
    }
    if(strcmp(msg,"logon success")==0){
        islogon = true;
        cout<<"登录成功"<<endl;
        return;
    }
    else{
        cout<<msg<<endl;
        if(strcmp(msg,"user is online")==0){
            throw "用户已在别处登录";
        }
        throw "账号不存在或密码错误";
    }
}

void Clinet::UserRegister(const char *phone, const char *passwd)
{
    char msg[125] = {0};
    send(socketFD, "Register request", 17, MSG_NOSIGNAL);

    cout << "Wait" << endl;
    recv(socketFD, msg, 125, 0);
    if (strcmp(msg, "Accept") != 0)
    {
        throw "Sever Accept error";
    }
    senduserdata data;
    strcpy(data.phone,phone);
    strcpy(data.passwd,passwd);
    send(socketFD,&data,sizeof(senduserdata),MSG_NOSIGNAL);
    bzero(msg,125);
    int ret = recv(socketFD,msg,125,0);
    if(ret<=0){
        throw "服务器断开连接";
    }
    
    if(strcmp(msg,"register success")==0){
        cout<<"注册成功"<<endl;
        islogon = true;
        return;
    }
    else{
        cout<<msg<<endl;
        throw "账号已存在";
    }
}

