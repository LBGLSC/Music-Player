#ifndef __DOULINKLIST_H__
#define __DOULINLIST_H__

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
typedef struct binode
{
    char data1[1024];
    char data2[1024];
    struct binode *prev;
    struct binode *next;
}Binode;

typedef struct bihead
{
    Binode *first;
    Binode *last;
    int num;
}Bihead;

Bihead *Add_DouLinklistHead(char *n,char *m,Bihead *head);//添加
void Print_Lisklist(Bihead*head);//输出双向链表

#endif
