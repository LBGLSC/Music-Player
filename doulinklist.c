#include"doulinklist.h"

Bihead *Add_DouLinklistHead(char *n,char *m,Bihead *head)//添加
{
    Binode *new = malloc(sizeof(Binode));
    strcpy(new -> data1,n);
    strcpy(new -> data2,m);
    new -> prev = NULL;
    new -> next = NULL;
    if(head -> num == 0)
    {
        head -> first = new;
        head -> last = new;
    }
    else
    {
        new -> prev = head -> last;
        head -> last -> next  = new;
        head -> last = new;
    }
    (head -> num)++;
    Binode *p = head -> first;
    return head;
}

void Print_Lisklist(Bihead *head)//输出双向链表
{
    Binode *p = head -> first;
    while(p)
    {
        printf("%s %s\n ",p -> data1,p -> data2);
        p = p -> next;
    }
}

