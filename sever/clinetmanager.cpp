#include "clinetmanager.h"
#include<cstring>
#include <iomanip>
#include<unistd.h>


list<userdata*> UserdataList;

ClientManager::ClientManager(cchp_t IP, int port,int socketfd):
    Client(IP,port,socketfd)
{
    m_phone = "0";
    m_passwd = "0";
    islogon = false;
    send_cancel_flag = false;
    this_it = UserdataList.end();
}

ClientManager::~ClientManager()
{
    if(islogon){
        (*this_it)->isOnline = false;
    }
}

void ClientManager::run()
{
    char *buf = new char[1024];
    while (1)
    {
        int ret = this->Recv(buf,1024);
        if(ret<=0){
            cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
            break;
        }
        //服务器处理各种请求
        cout<<buf<<":ret:" << ret <<endl;

        if(strcmp(buf,"Logon request")==0){
            if(islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }

            ret = this->logon();
            if(ret<0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;

        }
        else if(strcmp(buf,"Outlogon request")==0){
            if(!islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }

            ret = this->logonout();
            if(ret<0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;

        }
        else if(strcmp(buf,"Register request")==0){
            if(islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }
            cout<<__LINE__<<endl;

            ret = this->UserRegister();

            if(ret<0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;

        }
        else if(strcmp(buf,"Upload request")==0)
        {
            if(!islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }
            ret = this->RecvFile();
            if(ret<0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;
        }
        else if(strcmp(buf,"Download request")==0)
        {
            if(!islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }
            int ret = this->ProcessDownloadRequests();
            if(ret<=0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;
        }

        else if(strcmp(buf,"Getfinfo request")==0)
        {
            if(!islogon){
                cout<<__LINE__<<"Client has been tampered with"<<endl;
                break;
            }
            int ret = this->sendfileinfo(m_phone.c_str());
            if(ret<=0){
                cout<<__LINE__<<":"<<m_ip<<":"<<m_port<<"断开连接"<<endl;
                break;
            }
            continue;
        }
    }
    delete [] buf;
    buf = nullptr;
    delete this;
}

int ClientManager::logon()
{
    this->Send("Accept",7);
    senduserdata data;
    int ret = this->Recv(&data,sizeof(senduserdata));
    if(ret<=0){
        return ret;
    }
    //遍历检验
    list<userdata*>::iterator it;
    for(it=UserdataList.begin();it!=UserdataList.end();it++){
        if( strcmp( (*it)->phone,data.phone ) == 0 ){
            if((*it)->isOnline)
            {
                this->Send("user is online",15);
                return 1;
            }
            if( strcmp( (*it)->passwd,data.passwd ) == 0){
                m_phone = data.phone;
                m_passwd = data.passwd;
                this_it = it;
                (*this_it)->isOnline = true;
                islogon = true;
                this->Send("logon success", 14);
                cout<<(*this_it)->phone<<"is logon"<<endl;
                return 1;
            }
        }
    }
    this->Send("logon loss",11);
    cout<<"登录失败"<<endl;
    return 1;
}

int ClientManager::UserRegister()
{

    this->Send("Accept",7);

    senduserdata data;
    int ret = this->Recv(&data,sizeof(senduserdata));
    if(ret<=0){
        return ret;
    }

    list<userdata*>::iterator it;
    for(it=UserdataList.begin();it!=UserdataList.end();it++){
        if(strcmp( (*it)->phone, data.phone ) == 0){
            this->Send("user is exists",15);
            cout<<"用户已存在"<<endl;
            return 1;
        }
    }

    userdata* Ldata = new userdata;
    Ldata->isVIp=0;
    strcpy(Ldata->phone,data.phone);
    strcpy(Ldata->passwd,data.passwd);

    //插入链表
    UserdataList.push_front(Ldata);
    Ldata->isOnline = true;
    islogon = true;

    this->this_it = UserdataList.begin();

    m_phone = data.phone;
    m_passwd = data.passwd;

    cout<<(*this_it)->phone<<"@"<<(*this_it)->passwd<<endl;
    this->Send("register success", 17);
    FILE* fp = fopen(DATAFILE,"a");
    if(fp!=NULL){
        fprintf(fp,"%s@%s@%d\n", Ldata->phone, Ldata->passwd, Ldata->isVIp);
        fclose(fp);
    }

    mkdir(Ldata->phone,0777);
    return  1;
}

int ClientManager::logonout()
{
    this->Send("Accept",7);
    this->islogon = false;
    (*this_it)->isOnline = false;
    m_phone = "0";
    m_passwd = "0";
    return  1;
}

int ClientManager::RecvFile()
{
    this->Send("Accept",7);

    fileinfo_t fi;
    long long RecvSize = 0;
    int ret = this->Recv(&fi, sizeof(fileinfo_t));
    if(ret!=sizeof(fileinfo_t))
    {
        cout<<"数据接收不完整"<<endl;
        send(m_CilentsocketFD,"文件基本数据不完整",28,0);
        return ret;
    }

    char pathAndfilename[256+12];
    strcpy(pathAndfilename, m_phone.c_str());
    strcat(pathAndfilename,"/");
    strcat(pathAndfilename , fi.filename);
    cout<<pathAndfilename<<" size:"<<fi.size<<endl;
    char pathAndfilenameBat[256+12+4];
    sprintf(pathAndfilenameBat,"%s.bat",pathAndfilename);

    int FileFD = open(pathAndfilenameBat,  O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(FileFD==-1)
    {
        this->Send("sever error",12);
        return 1;
    }
    this->Send("OK",3);

    char *buf = new char[MAXSIEZ];
    cout << setiosflags(ios::fixed); //只有在这项设置后，setprecision才是设置小数的位数。
    while (1)
    {
        ret = this->Recv(buf,MAXSIEZ,false);
        if(ret<=0 || strcmp(buf,"cancel")==0 )
        {
            delete[] buf;
            close(FileFD);
            remove(pathAndfilenameBat);
            cout<<"客户端取消请求或发生未知错误"<<endl;
            return ret;
        }
        //cout<<"dataBit:"<<ret<<endl;
        int wret = write(FileFD,buf,ret);
        RecvSize += wret;

        if(RecvSize == fi.size){
            this->Send("Recv finsh",11);
            remove(pathAndfilename);
            rename(pathAndfilenameBat,pathAndfilename);
            cout<<"Recv finsh:　";
            cout << setprecision(2) << RecvSize*1.0/MAXSIEZ<<"KB/"<<fi.size*1.0/MAXSIEZ<<"KB"<< endl; //输出2位小数
            break;
        }
    }
    delete[] buf;
    close(FileFD);
    return 1;
}

int ClientManager::ProcessDownloadRequests()
{
    this->Send("Accept",7);
    char filename[125]={0};
    int ret = this->Recv(filename,125);
    if(ret<=0)
    {
        return  ret;
    }
    this->SendFile(filename);
    return 1; //发送完返回1
}

void ClientManager::SendFile(cchp_t filename)
{

    char pathAndfilename[125+12];
    strcpy(pathAndfilename , m_phone.c_str());
    strcat(pathAndfilename,"/");
    strcat(pathAndfilename,filename);
    cout<<pathAndfilename<<endl;
    int FileFd = open(pathAndfilename,O_RDONLY);
    if(FileFd==-1){
        this->Send("NO file",8);
        return;
    }
    else{
        this->Send("has file",9);
    }
    //发送文件大小
    struct stat fileInfo;
    fstat(FileFd,&fileInfo);
    this->Send(&fileInfo.st_size, sizeof(fileInfo.st_size));
    //等待客户端回应
    char msg[125]={0};
    this->Recv(msg,125);
    if (strcmp(msg, "OK") != 0)
    {
        close(FileFd);
        cout<< "Client system error"<<endl;
        return;
    }

    char *buf = new char[MAXSIEZ];
    int sbit = 0;
    //监听客户端取消请求信号
    thread t1(&ClientManager::SendingCancel,this);

    cout << setiosflags(ios::fixed); // 只有在这项设置后，setprecision才是设置小数的位数
    while (!send_cancel_flag) {
        int rret = read(FileFd,buf, MAXSIEZ);
        if(rret<0){
            cout << "send over" << endl;
            break;
        }
        if(!send_cancel_flag){
            int sret = this->Send(buf,rret);
            if(sret<=0)
            {
                break;
            }
            sbit = sbit + sret;

        }

    }
    cout<<"send over: ";
    cout << setprecision(2) << sbit*1.0/MAXSIEZ<<"KB/"<<fileInfo.st_size*1.0/MAXSIEZ<<"KB"<< endl; //输出2位小数
    delete [] buf;
    close(FileFd);
    //回收线程
    pthread_cancel(t1.native_handle());
    t1.join();
    send_cancel_flag = false;
}

void ClientManager::SendingCancel()
{
    char msg[7] ={0};
    this->Recv(msg,7);
    cout<<msg<<endl;
    if(strcmp(msg,"cancel")==0)
    {
        send_cancel_flag = true;
    }
    pause();
}

int ClientManager::sendfileinfo(const char* dir)
{
    this->Send("Accept",7);

    DIR* dirp = opendir(dir);

    while (1) {

        // 读取目录项
        struct dirent *Info = readdir(dirp);
        if (Info == NULL)
        {
            fprintf(stderr, "read dir %s error:%s\n", dir, strerror(errno));
            this->Send("over",5);
            break;
        }
        if (strcmp(Info->d_name, ".") && strcmp(Info->d_name, "..")){
            FileInfo info;
            info.type_t = Info->d_type;
            strcpy(info.filename,Info->d_name);
            this->Send(&info,sizeof (FileInfo));

            char msg[12]={0};
            int ret = this->Recv(msg,12);
            if(ret<=0){
                cout<<__LINE__<<endl;
                closedir(dirp);
                return ret;
            }

            if(strcmp(msg,"next")==0){
                continue;
            }
            else {
                cout<<"dir over"<<endl;
                closedir(dirp);
                return 1;
            }
        }

    }
}


