#include "music.h"

#define FIFO_PATH "./music.fifo"

Bihead *find(const char *dirname)//寻找目录下音乐文件
{
    Bihead *head = malloc(sizeof(Bihead));
    head -> first =NULL;
    head -> last = NULL;
    head -> num = 0;
    DIR *dir = opendir(dirname);
    struct dirent *dirp = NULL;
    struct stat st;
    while(dirp = readdir(dir))
    {
        if(strcmp(dirp -> d_name,".") == 0 || strcmp(dirp -> d_name,"..") == 0)
        {
            continue;
        }
        char stname[1024];
        sprintf(stname,"%s/%s",dirname,dirp ->d_name);
        int fd = stat(stname,&st);
        if(fd == -1)
        {
            perror("获取文件属性失败\n");
        }
        if(S_ISREG(st.st_mode))
        {
            //printf("%s\n",dirp ->d_name);
            char *name = malloc(1024);
            strcpy(name,stname);
            int num;
            char bmpname[1024] = {0};
            char mpname[1024] = {0};
            int flag = 0;
            while(*name)
            {
                if(flag == 0)
                {
                    strncat(bmpname,name,1);
                    strncat(mpname,name,1);
                }
                if(strncmp(name,".",1) == 0 && strncmp((name + 1),"m",1) == 0  && strncmp((name + 2),"p",1) == 0
                && (strncmp((name + 3),"3",1) == 0 || strncmp((name + 3),"4",1) == 0))
                {
                    flag = 1;
                    num = 0;
                }
                num++;
                name++;
            }
            if(num == 4)
            {
                //printf("%s %s\n",bmpname,mpname);
                strcat(bmpname,"bmp");
                strcat(mpname,"mp3");
                //printf("qqq%s %s\n",mpname,bmpname);
                head = Add_DouLinklistHead(mpname,bmpname,head);
            }
        }
    }

    return head;
}

int face(const char *path)//屏幕显示
{
    clean(0x1E1E1E);//将屏幕变为color色
    for(int x = 0;x < 800;x++)
    {
        for(int y = 0; y < 50;y++)
        {
            Lcd_DrawPoint(x,y,0xFF3030);//将屏幕(x,y)处的像素点显示成color颜色
        }
    }
    //让儿子去播放音乐
    int fd = Lcd_DisplayBmp(0,50,path);
    //printf("%s %s \n",p -> data1,p -> data2);
    if(fd == -1)
    {
        perror("没有此图片\n");
        return -1;
    }
    Lcd_DisplayBmp(450,390,"./song/prev.bmp");//上一首
    Lcd_DisplayBmp(580,380,"./song/stop.bmp");//停止
    Lcd_DisplayBmp(720,390,"./song/next.bmp");//下一首
    Lcd_DisplayBmp(450,70,"./song/jian.bmp");//减小音量
    Lcd_DisplayBmp(570,70,"./song/yes.bmp");//声音
    Lcd_DisplayBmp(720,70,"./song/jia.bmp");//增大音量
    return 0;
}

int built_fifo(char *str)//建立管道
{
    int fd_fifo=open(FIFO_PATH,O_RDWR);
    char buf[1024];
    strcat(buf,str);
    printf("%s\n",buf);
    int ret = write(fd_fifo,buf,strlen(buf));//2.往有名管道中写入数据
}

int play_music(Bihead *head)//播放音乐
{
    Lcd_Init();
    Binode *p = head -> first;
    unlink(FIFO_PATH);					//如果明明管道存在，则先删除
	int fife = mkfifo(FIFO_PATH,O_CREAT|0666);
    if(fife == -1)
    {
        perror("mkfifo");
    }
    built_fifo("volume 10 1\n");
    while(1)
    {
        pid_t pid1 =fork();
        if(pid1 == -1)
        {
            perror("创建子进程失败\n");
            return -1;
        }
        if(pid1  == 0)//在下一级的子进程
        {
            face(p -> data2);
            execlp("mplayer","mplayer",
            "-slave","-quiet",
            "-input","file=/IOT/long/1.fifo",
            p -> data1,NULL);//songname
        }
        else if(pid1 > 0 )//在下一级的父进程
        {
            pid_t pid2 =fork();
            if(pid2 == -1)
            {
                perror("创建子进程失败\n");
                return -1;
            }
            if(pid2  == 0)//在父进程的子进程
            {
                int num  = 0;
                while(1)
                {
                    Point p1 = Get_Touch();
                    if(923 < p1.x && p1.x < 1012 && 487 < p1.y && p1.y < 575)
                    {
                        if(p == NULL)
                        {
                            p = head -> first;
                        }
                        kill(pid1,SIGKILL);
                        break;
                    }
                    if(743 < p1.x && p1.x < 846 && 475 < p1.y && p1.y < 575)
                    {
                        built_fifo("pause\n");
                    }
                    if(576 < p1.x && p1.x < 666 && 487 < p1.y && p1.y < 575)
                    {
                        p = p -> prev;
                        if(p == NULL)
                        {
                            p = head -> last;
                        }
                        p = p -> prev;
                        if(p == NULL)
                        {
                            p = head -> last;
                        }
                        kill(pid1,SIGKILL);
                        break;
                        printf("%d %d\n",p1.x,p1.y);
                    }
                    if(923 < p1.x && p1.x < 1012 && 87 < p1.y && p1.y < 175)//增加音量
                    {
                        built_fifo("volume +5000\n");
                    }
                    if(743 < p1.x && p1.x < 846 && 87 < p1.y && p1.y < 175)//增加音量
                    {
                        if(num % 2 == 0)
                        {
                            built_fifo("mute 1\n");
                            Lcd_DisplayBmp(570,70,"./song/no.bmp");//声音
                            num++;
                        }
                        else if(num % 2 != 0)
                        {
                            built_fifo("mute 0\n");
                            Lcd_DisplayBmp(570,70,"./song/yes.bmp");//声音
                            num++;
                        }
                    }
                    if(576 < p1.x && p1.x < 666 && 87 < p1.y && p1.y < 175)//减小音量
                    {
                        built_fifo("volume -5000\n");
                    }
                } 
            }
            if(pid2 > 0)//在父进程的父进程
            {
                wait(NULL);
                p = p -> next;
            }
            wait(NULL);
            printf("over\n");
            p = p -> next;
            built_fifo("volume 10 1\n");
            if(p == NULL)//循环播放
            {
                p = head -> first;
            }   
        }
    }
}

