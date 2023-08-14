#include "panmain.h"

list<FileInfo*> FIlist;
list<FileInfo*> localFIlist;
queue<tasklist*> TaskList;
deque<tasklist*> WaitList;
long long c_size=0;
long long t_size=0;
char *nowtaskname = nullptr;
pthread_mutex_t TNmutex;
pthread_mutex_t WLmutex;

extern bool cleanflag;
PanMain::PanMain()
{
    m_dircount = 0;
    m_pandircount = 0;
    n_pages = 1;
    n_Ppages = 1;
    max_pages = max_Ppages = 0;
    listnum = 1;
    changepath = false;
    pthread_mutex_init(&TNmutex,NULL);
    pthread_mutex_init(&WLmutex,NULL);
    bzero(m_nowpath,256);
    strcpy(m_nowpath, "/zayu/netpan");
    thread taskThread(&PanMain::taskthread,this);
    m_tasktid = taskThread.native_handle();
    taskThread.detach();

    thread taskThread1(&PanMain::listThread,this);
    m_stasktid = taskThread1.native_handle();
    taskThread1.detach();

    thread taskThread2(&PanMain::showWaitThread,this);
    m_swtid = taskThread2.native_handle();
    taskThread2.detach();
}

PanMain::~PanMain()
{
    pthread_cancel(m_tasktid);
    pthread_cancel(m_stasktid);
    pthread_cancel(m_swtid);
}


bool PanMain::run()
{
    Bmp bmp;
    MFont* font = MFont::instance();

    bmp.setFileName("main.bmp");
    bmp.show();

    this->Recvfileinfo();
    showlist();
    this->GetLocalFileInfo(m_nowpath);
    ShowLocalList();

    Touch touch;
    int x,y;
    int lchoosepox = 0;
    int lchoosepoy = 0;
    int Plchoosepox = 0;
    int Plchoosepoy = 0;
    const char *sendFN = nullptr;
    const char *recvFN = nullptr;
    while (1) {
        char pg[12];
        sprintf(pg,"%d/%d",n_pages,max_pages);
        font->show(pg,24,40,24,MFont::WHITE,494,448-24,0,0,0x00);
        if(listnum==1){
            sprintf(pg,"%d/%d",n_Ppages,max_Ppages);
            font->show(pg,14,30,12,MFont::WHITE,115,418-12,0,0,0x00);
        }

        touch.wait(x,y);
        //右边文件列表
        if(x>290&&y>118 && x<775&&y<420)
        {
            int p = (n_pages-1)*12;
            int c = (x-290)/120 + (y-118)/100*4;
            int i = p+c;
            if(i>m_dircount-1)
            {
                continue;
            }
            list<FileInfo*>::iterator it =localFIlist.begin();
            while (i) {
                it++;
                i--;
            }
            //点到文件夹
            if((*it)->type_t==4)
            {
                cout<<__LINE__<<endl;
                if( strcmp((*it)->filename,"..")==0 )
                {
                    char *p = strrchr(m_nowpath,'/');
                    p[0]='\0';
                    if(strlen(m_nowpath)==0){
                      strcpy(m_nowpath,"/");
                    }
                }
                else{
                    if(strcmp(m_nowpath,"/")!=0){
                        strcat(m_nowpath,"/");
                    }
                    strcat(m_nowpath, (*it)->filename);
                }
                cleanlist(1);
                GetLocalFileInfo(m_nowpath);
                bmp.setFileName("fs.bmp");
                bmp.show(285,119);
                n_pages = 1;
                sendFN = nullptr;
                ShowLocalList();
                changepath = true;
            }
            //点到文件
            else if((*it)->type_t==8)
            {
                sendFN = (*it)->filename;
                if(lchoosepox!=0 || lchoosepoy!=0){
                    bmp.setFileName("file.bmp");
                    bmp.show(lchoosepox, lchoosepoy);
                }

                bmp.setFileName("c_file.bmp");
                bmp.show((c%4)*120+290, c/4*100+118);

                lchoosepox = (c%4)*120+290;
                lchoosepoy = c/4*100+118;
            }
        }
        //左边文件列表
        else if (x>20&&y>100 && x<262&&y<408 && listnum==1)
        {
            int p = (n_Ppages-1)*6;
            int c = (x-20)/120 + (y-100)/100*2;
            int i = p+c;
            cout<<"i:"<<i<<endl;
            if(i>m_pandircount-1)
            {
                continue;
            }
            list<FileInfo*>::iterator it = FIlist.begin();
            while (i) {
                it++;
                i--;
            }
            if((*it)->type_t==8)
            {
                recvFN = (*it)->filename;
                if(Plchoosepox!=0 || Plchoosepoy!=0){
                    bmp.setFileName("file.bmp");
                    bmp.show(Plchoosepox, Plchoosepoy);
                }

                bmp.setFileName("c_file.bmp");
                bmp.show((c%2)*120+20, c/2*100+100);

                Plchoosepox = (c%2)*120+20;
                Plchoosepoy = c/2*100+100;
            }
        }
        //右边上一页
        else if(x>608&&y>76 && x<646&&y<104)
        {
            if(n_pages==1||n_pages==0){
                continue;
            }
            n_pages-=1;
            sendFN = nullptr;
            lchoosepox = 0;
            lchoosepoy = 0;
            bmp.setFileName("fs.bmp");
            bmp.show(285,119);
            ShowLocalList();
        }
        //右边下一页
        else if(x>668&&y>77 && x<713&&y<102){
            if(n_pages==max_pages){
                continue;
            }
            n_pages+=1;
            sendFN = nullptr;
            lchoosepox = 0;
            lchoosepoy = 0;
            bmp.setFileName("fs.bmp");
            bmp.show(285,119);
            ShowLocalList();
        }
        //刷新
        else if(x>204&&y>425  && x<248&&y<480)
        {
            if(listnum==1)
            {
                bmp.setFileName("fs1.bmp");
                bmp.show(8,100);
                showlist();
                recvFN =nullptr;
                max_Ppages = m_pandircount/6;
                if(m_pandircount%6!=0){
                    max_Ppages++;
                }

            }
            if(!changepath){
                sendFN = nullptr;
                bmp.setFileName("fs.bmp");
                bmp.show(285,119);
                ShowLocalList();
                max_pages = m_dircount/12;
                if(m_dircount%12!=0){
                    max_pages+=1;
                }
            }

        }
        //左边上一页
         else if(x>67&&y>0 && x<111&&y<45 && listnum==1)
        {
            if(n_Ppages==1||n_Ppages==0){
                continue;
            }
            n_Ppages-=1;
            Plchoosepox = 0;
            Plchoosepoy = 0;
            bmp.setFileName("fs1.bmp");
            bmp.show(8,100);
            showlist();
            recvFN =nullptr;
        }
        //左边下一页
        else if(x>156&&y>0 && x<206&&y<45 && listnum==1){
            if(n_Ppages==max_Ppages){
                continue;
            }
            n_Ppages+=1;
            Plchoosepox = 0;
            Plchoosepoy = 0;
            bmp.setFileName("fs1.bmp");
            bmp.show(8,100);
            showlist();
            recvFN =nullptr;
        }
        //下载
        else if(x>280&&y>45 && x<348&&y<80)
        {
            if(recvFN==nullptr){
                font->show("请选择文件!",24,150,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            if(!Clinet::severOnline()){
                font->show("与服务器断开连接!",24,180,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            if(TaskList.size()==5){
                font->show("任务队列已满!",24,158,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            tasklist* newtask = new tasklist;
            strcpy(newtask->path,m_nowpath);
            strcpy(newtask->filename,recvFN);
            newtask->TaskType = Upload;
            TaskList.push(newtask);
            pthread_mutex_lock(&WLmutex);
            WaitList.push_back(newtask);
            pthread_mutex_unlock(&WLmutex);
            changepath = false;
            font->show("已加入任务队列!",24,170,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
            sleep(1);
            bmp.setFileName("hide1.bmp");
            bmp.show(355,0);

        }
        //上传
        else if(x>502&&y>25 && x<571&&y<77 )
        {
            if(sendFN==nullptr){
                font->show("请选择文件!",24,150,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            if(!Clinet::severOnline()){
                font->show("与服务器断开连接!",24,180,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            if(TaskList.size()==5){
                font->show("任务队列已满!",24,158,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
                sleep(1);
                bmp.setFileName("hide1.bmp");
                bmp.show(355,0);
                continue;
            }
            tasklist* newtask = new tasklist;
            strcpy(newtask->path,m_nowpath);
            strcpy(newtask->filename,sendFN);
            newtask->TaskType = Download;
            TaskList.push(newtask);
            pthread_mutex_lock(&WLmutex);
            WaitList.push_back(newtask);
            pthread_mutex_unlock(&WLmutex);
            font->show("已加入任务队列!",24,170,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
            sleep(1);
            bmp.setFileName("hide1.bmp");
            bmp.show(355,0);
        }
        //网盘列表
        else if(x>4&&y>425  && x<66&&y<480){
            listnum= 1;
            bmp.setFileName("list1.bmp");
            bmp.show(0,46);
            showlist();
            char pg[12];
            sprintf(pg,"%d/%d",n_Ppages,max_Ppages);
            font->show(pg,14,30,12,MFont::WHITE,115,418-12,0,0,0x00);

        }
        //任务列表
        else if(x>78&&y>425  && x<128&&y<480){

            bmp.setFileName("list2.bmp");
            bmp.show(0,46);
            listnum = 2;
            recvFN = nullptr;
        }
        //取消当前任务
        else if(x>134&&y>425  && x<190&&y<480)
        {
            cleanflag = true;
            font->show("取消中..",24,160,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
            sleep(3);
            bmp.setFileName("hide1.bmp");
            bmp.show(355,0);
            if(listnum == 2){
                font->show("X",18,55,24,MFont::WHITE, 190,105,2,0,MFont::RED);
            }
        }

        //退出登录
        else if(x>713&&y>0 && x<800&&y<42)
        {
            cleanflag = true;
            font->show("退出登录中..",24,160,24,MFont::MCOLOR,355,12,0,0,MFont::WHITE);
            cleanlist(1);
            cleanlist(2);
            sleep(3);
            bmp.setFileName("hide1.bmp");

            Clinet::instance()->Sendmsg("Outlogon request",17);
            char msg[12]={0};
            Clinet::instance()->Recvmsg(msg,12);
            if(strcmp(msg,"Accept")==0){
                Clinet::SetIslogon(false);
                cout<<"退出登录"<<endl;
            }
            return true;
        }
    }


}

void PanMain::Recvfileinfo(Clinet *clinet)
{
    clinet->Sendmsg("Getfinfo request",17);
    char msg[125]={0};

    clinet->Recvmsg(msg,125);
    if (strcmp(msg, "Accept") != 0)
    {
        throw "Sever Accept error";
    }
    m_pandircount = 0;
    while(1){

        FileInfo * node = new FileInfo;
        int ret = clinet->Recvmsg(node, sizeof (FileInfo));
        if(ret != sizeof (FileInfo) ){
            cout<<"fi recv over"<<endl;
            delete node;
            break;
        }
        FIlist.push_back(node);
        m_pandircount++;
        clinet->Sendmsg("next",5);
    }
    max_Ppages = m_pandircount/6;
    if(m_pandircount%6!=0){
        max_Ppages++;
    }
    if(max_Ppages == 0){
        max_Ppages = 1;
    }
}

void PanMain::GetLocalFileInfo(const char *dir)
{
    DIR* dirp = opendir(dir);
    if(dirp==NULL){
        fprintf(stderr, "open dir [%s] error:%s\n", dir, strerror(errno));
        return;
    }
    m_dircount = 0;
    while (1) {
        // 读取目录项
        struct dirent *Info = readdir(dirp);

        if (Info == NULL)
        {
            fprintf(stderr, "read dir %s error:%s\n", dir, strerror(errno));
            break;
        }

        if (strcmp(Info->d_name, ".") && (Info->d_type==8||Info->d_type==4))
        {
            FileInfo* info = new FileInfo;            
            info->type_t = Info->d_type;            
            strcpy(info->filename,Info->d_name);           
            localFIlist.push_back(info);
            m_dircount++;
        }
    }
    max_pages = m_dircount/12;
    if(m_dircount%12!=0){
        max_pages+=1;
    }
    closedir(dirp);
}

void PanMain::showlist()
{
    Bmp bmp;
    MFont* font = MFont::instance();

    list<FileInfo*>::iterator it=FIlist.begin();
    int i = 0;
    int num = (n_Ppages-1)*6;
    while (num--)
    {
        i++;
        if(i>=m_pandircount){
            break;
        }
        it++;

    }
    i=0;
    for(; it!=FIlist.end(); it++)
    {
        if(i>=6){
            break;
        }
        if((*it)->type_t==4){
            bmp.setFileName("dir.bmp");
            bmp.show( ((i%2)*120)+20 , ((i/2)*100)+100);
        }
        else if((*it)->type_t==8){
            bmp.setFileName("file.bmp");
            bmp.show( ((i%2)*120)+20 , ((i/2)*100)+100);
        }
        int len = strlen((*it)->filename);
        if(len<7)
        {
            int k =7-len;
            char newi[8]={0};
            for(int i=0;i<k/2;i++){
                newi[i]=' ';
            }
            strcat(newi,(*it)->filename);
            font->show(newi, 18, 100,24, MFont::WHITE, ((i%2)*120+20)+20, ((i/2)*100+100)+80, 0,3, 0x00);
        }

        else {
            font->show((*it)->filename,18, 100,24, MFont::WHITE, ((i%2)*120+20)+20, ((i/2)*100+100)+80, 0,3, 0x00);
        }
        i++;
    }
}

void PanMain::ShowLocalList()
{
    Bmp bmp;
    MFont* font = MFont::instance();
    list<FileInfo*>::iterator it=localFIlist.begin();;
    int i = 0;
    int num = (n_pages-1)*12;
    while (num--)
    {
        i++;
        if(i>=m_dircount){
            break;
        }
        it++;

    }
    i=0;
    for( ;it!=localFIlist.end(); it++)
    {
        if(i>=12) {
            break;
        }
        if((*it)->type_t==4){
            bmp.setFileName("dir.bmp");
            bmp.show( ((i%4)*100+(i%4)*20)+290 , ((i/4)*100)+118);
        }
        else{
            bmp.setFileName("file.bmp");
            bmp.show( ((i%4)*100+(i%4)*20)+290 , ((i/4)*100)+118);
        }
        int len = strlen((*it)->filename);
        if(len<7)
        {
            int k =7-len;
            char newi[8]={0};
            for(int i=0;i<k/2;i++){
                newi[i]=' ';
            }
            strcat(newi,(*it)->filename);           
            font->show(newi, 18, 100,24, MFont::WHITE, (  ( (i%4)*100+(i%4)*20) )+290 +20, ((i/4)*100+118)+80, 0,3, 0x00);

        }

        else {        
            font->show((*it)->filename,18, 100,24, MFont::WHITE, (( (i%4)*100+(i%4)*20) )+290 +20, ((i/4)*100+118)+80, 0,3, 0x00);
        }
        i++;
    }
}

void PanMain::cleanlist(int n)
{
    list<FileInfo*>::iterator it;
    if(n==1)
    {
        for(it=localFIlist.begin(); it!=localFIlist.end(); it++)
        {
            delete *it;
        }
        localFIlist.clear();
    }
    else
    {
        for(it=FIlist.begin(); it!=FIlist.end(); it++)
        {
            delete *it;
        }
        FIlist.clear();
    }

}

void PanMain::taskthread()
{
    Bmp bmp;
    bmp.setFileName("fs3.bmp");
    while(1)
    {
        usleep(20);
        if(TaskList.empty())
        {

            continue;
        }
        tasklist* t = TaskList.front();
        pthread_mutex_lock(&WLmutex);
        if(!WaitList.empty()){
             WaitList.pop_front();
             if(listnum==2){
                 bmp.show(20,140+46);
             }

        }
        pthread_mutex_unlock(&WLmutex);

        nowtaskname = t->filename;
        if(t->TaskType==Download)
        {
            try {
                Clinet::instance()->Sendfile(t->filename,t->path);
                if(!cleanflag){
                    FileInfo* addnew = new FileInfo;
                    strcpy(addnew->filename,t->filename);
                    addnew->type_t = 8;
                    FIlist.push_back(addnew);
                    m_pandircount+=1;
                }
            } catch (const char* str) {
                cout<<str<<endl;
            }
        }
        else if(t->TaskType==Upload)
        {
            try {
                Clinet::instance()->Recvfile(t->filename,t->path);
                if(!cleanflag){
                    FileInfo* addnew = new FileInfo;
                    strcpy(addnew->filename,t->filename);
                    addnew->type_t = 8;
                    localFIlist.push_back(addnew);
                    m_dircount+=1;
                }
            } catch (const char* str) {
                cout<<str<<endl;
            }
        }
        if(!cleanflag){
            c_size = t_size = 1;
        }
        TaskList.pop();
        pthread_mutex_lock(&TNmutex);
        nowtaskname = nullptr;
        pthread_mutex_unlock(&TNmutex);
        delete t;
    }
}

void PanMain::listThread()
{
    UMFont font;
    while (1)
    {
        usleep(20);
        if( listnum == 1 || TaskList.empty())
        {

            continue;
        }
        pthread_mutex_lock(&TNmutex);
        if(nowtaskname!=nullptr)
        {
            font.show(nowtaskname,24,175,24,MFont::WHITE,20,105,0,0,MFont::GREEN);
            char buf[10]={0};
            double p = (c_size*1.0/t_size);
            p *= 100;
            sprintf(buf,"%.1f",p);
            strcat(buf,"%");
            font.show(buf,18,55,24,MFont::WHITE, 190,105,0,0,MFont::RED);

        }
        pthread_mutex_unlock(&TNmutex);
    }
}

void PanMain::showWaitThread()
{
    UMFont font;
    while (1)
    {
        usleep(20);
        if( listnum == 1 || WaitList.empty())
        {
            continue;
        }

        pthread_mutex_lock(&WLmutex);
        deque<tasklist*>::iterator it = WaitList.begin();
        int i =0;
        for(;it!=WaitList.end();it++)
        {
            font.show((*it)->filename,24,210,24,MFont::WHITE,20,(i*40)+140+46,0,0,MFont::RED);
            i++;
        }
        pthread_mutex_unlock(&WLmutex);
    }
}

