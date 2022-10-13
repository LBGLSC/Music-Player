#ifndef __HEAD_H__
#define __HEAD_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <pthread.h>
#include <time.h>
#include<sys/wait.h>


typedef struct point //点触结构体
{
	int x;//点触横坐标
	int y;//点触纵坐标
}Point;

int Lcd_Init(void);//对LCD屏进行初始化 @return: 初始化成功返回0,初始化失败返回-1
void Recover_Source(void);//回收资源

int Lcd_DisplayBmp(int x0,int y0,const char *bmpname);//在(x0,y0)处显示bmpname指定的BMP文件 @return : 显示成功返回0,显示失败返回-1
void Lcd_DrawWord(int x0,int y0,int w,int h,char *data,int color);//在位置为(x0,y0)处显示宽为w,高为h,颜色color的汉字

void Lcd_DrawPoint(int x,int y,int color);//将屏幕(x,y)处的像素点显示成color颜色	
void clean(int color);//将屏幕变为color色

Point Get_Touch(void);//进行一次点触，返回结构体(p.x横坐标 p.y纵坐标)

#endif

