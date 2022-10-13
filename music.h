#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "head.h"
#include "doulinklist.h"
#include <dirent.h>


Bihead *find(const char *dirname);
int play_music(Bihead *head);
int face(const char *path);
int built_fifo(char *str);
void music_touch();//触屏


#endif
