#include "music.h"

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        printf("输入过少\n");
    }
    Bihead *head = find(argv[1]);
    Print_Lisklist(head);//输出双向链表
    play_music(head);
}
