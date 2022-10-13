#include "head.h"

int Fd_Lcd = -1;//屏幕文件的文件描述符

int *Plcd = NULL;//映射区域的地址

#define LCD_PATH "/dev/fb0"//屏幕文件的路径
#define TOUCH_PATH "/dev/input/event0"//点触文件路径

#define LCD_WIDTH 	800//屏幕文件的大小
#define LCD_HEIGHT  480



int Lcd_Init(void)
{
	//打开屏幕文件
	Fd_Lcd = open(LCD_PATH,O_RDWR);
	if(Fd_Lcd == -1)
	{
		perror("打开屏幕文件失败");
		return -1;
	}

	//将屏幕文件映射到内存中去
	Plcd = mmap(NULL,LCD_WIDTH*LCD_HEIGHT*4,PROT_READ | PROT_WRITE,MAP_SHARED,Fd_Lcd,0);
	if(Plcd == MAP_FAILED)
	{
		perror("映射屏幕文件失败");
		return -1;
	}
	return 0;
}

void Lcd_DrawPoint(int x,int y,int color)
{
	if(x >= 0 && x < 800 && y >= 0 && y < 480)
	{
		*(Plcd + 800 * y + x) = color;
	}
}

int Lcd_DisplayBmp(int x0,int y0,const char *bmpname)
{
	//1.打开BMP文件
	int fd_bmp = open(bmpname,O_RDONLY);
	if(fd_bmp == -1)
	{
		perror("打开BMP文件失败");
		return -1;
	}

	//2.判断bmpname是不是BMP文件
	unsigned char buf[2] = {0};
	int ret = read(fd_bmp,buf,2);
	if(ret == -1)
	{
		perror("读取BMP文件魔数失败");
		return -1;
	}

	if(buf[0] == 0x42 && buf[1] == 0x4D)
	{
		//printf();
	}
	else
	{
		printf("%s is not bmp file,bye bye!\n",bmpname);
		close(fd_bmp);
		return -1;
	}

	//3.读取当前bmp文件的宽度
	//将光标偏移至宽度数据之前
	lseek(fd_bmp,0x12,SEEK_SET);
	int width;
	ret = read(fd_bmp,&width,4);
	if(ret == -1)
	{
		perror("读取BMP文件宽度失败");
		return -1;
	}

	//4.读取当前bmp文件的高度
	//将光标偏移至高度数据之前
	lseek(fd_bmp,0x16,SEEK_SET);
	int height;
	ret = read(fd_bmp,&height,4);
	if(ret == -1)
	{
		perror("读取BMP文件高度失败");
		return -1;
	}

	//5.读取当前bmp文件的色深
	//将光标偏移至色深数据之前
	lseek(fd_bmp,0x1C,SEEK_SET);
	unsigned short depth;
	ret = read(fd_bmp,&depth,2);
	if(ret == -1)
	{
		perror("读取BMP文件色深失败");
		return -1;
	}

	//求出BMP文件的像素数组一行的大小
	int bytesbmpline = abs(width) * (depth / 8);

	unsigned int laizi = 0;
	//求出赖子的个数
	if(bytesbmpline % 4)
	{
		laizi = 4 - bytesbmpline % 4;
	}

	//求出用来保存像素数组的空间一行的字节数
	int bytespiexlline = bytesbmpline + laizi;

	//求出用来保存像素数组的空间大小
	int size = abs(height) * bytespiexlline;

	//把空间开辟出来
	unsigned char *piexl = malloc(size);

	//将像素数组读取到开辟出来的空间中去
	lseek(fd_bmp,54,SEEK_SET);
	ret = read(fd_bmp,piexl,size);
	if(ret == -1)
	{
		perror("读取像素数组失败");
		return -1;
	}

	//对数据进行处理
	//if depth == 24 每次从空间中取三个字节整合形成一个像素点的颜色值
	//将其显示到屏幕的对应位置上去
	int x,y;
	unsigned char a,r,g,b;
	unsigned int color;
	int i = 0;
	for(y = 0;y < abs(height);y++)
	{
		for(x = 0;x < abs(width);x++)
		{

			//获取BMP文件中坐标为(x,y)处的像素点的颜色分量
			b = piexl[i++];
			g = piexl[i++];
			r = piexl[i++];
			if(depth == 24)
			{
				a = 0x00;
			}
			else if(depth == 32)
			{
				a = piexl[i++];
			}
			else
			{
				printf("%s文件色深不支持!\n",bmpname);
				return -1;
			}

			//将像素点的三个颜色分量合成一个像素点的值
			color = (a << 24 | r << 16 | g << 8 | b);

			//将BMP文件中坐标为(x,y)处的像素点显示到屏幕的对应位置上去
			Lcd_DrawPoint(x0 + x,y0 + abs(height) - y - 1,color);
		}
		i = i + laizi;
	}

	close(fd_bmp);
}

/*
	Recover_Source:回收资源
*/
void Recover_Source(void)
{
	close(Fd_Lcd);
	munmap(Plcd,LCD_WIDTH*LCD_HEIGHT*4);
}

void Lcd_DrawWord(int x0,int y0,int w,int h,char *data,int color)
{
	int i,k;
	
	for(i = 0;i < w*h/8;i++)
	{
		for(k = 0;k < 8;k++)
		{
			if((data[i] << k) & 0x80)
			{
				Lcd_DrawPoint(x0 + (i*8+k) % w,y0 + i/ (w/8),color);
			}
		}			
	}
}

void clean(int color)
{
	for(int y = 0;y < 480;y++)
	{
		for(int x = 0;x <800;x++)
		{
			Lcd_DrawPoint(x,y,color);
		}
	}
}

Point Get_Touch(void)
{
	Point p;
	p.x = p.y = -1;
	//打开触摸屏文件
	int fd_touch = open(TOUCH_PATH,O_RDONLY);
	if(fd_touch == -1)
	{
		perror("打开触摸屏文件失败");
		return p;
	}

	struct input_event ev;
	//读取触摸屏文件的内容
	while(1)
	{
		int ret = read(fd_touch,&ev,sizeof(ev));
		if(ret != sizeof(ev))
		{
			continue;
		}

		if(ev.type == EV_ABS && ev.code == ABS_X)
		{
			//意味这是x轴的坐标
			p.x = ev.value;
		}
		else if(ev.type == EV_ABS && ev.code == ABS_Y)
		{
			//意味这是y轴的坐标
			p.y = ev.value;			
		}
		else if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
		{
			close(fd_touch);
			return p;
		}
		// if(p.x >= 0 && p.y >= 0)
		// {
		// 	close(fd_touch);
		// 	return p;
		// }
	}	
}
