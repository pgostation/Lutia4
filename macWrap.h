/*
 *  macWrap.h
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */
 
#ifndef _MACWRAP_H_
#define _MACWRAP_H_

/////////////////
/*** コンパイラ ***/
/////////////////

#ifdef __MRC__
	/* MPW's MrC */
	#define true 1
	#define false 0
#else
	/* other */
	#include <unistd.h>
#endif

/////////////////
/*** typedef ***/
/////////////////

#ifdef __CARBON__
typedef SndListHandle tSound;
typedef SndChannelPtr tChannel;
typedef GWorldPtr tPlane;
typedef RGBColor tColor;
typedef Point tPoint;
typedef Rect tRect;
#else
typedef char **tSound;
typedef struct{int a,b;} *tChannel;
typedef struct{int a,b;} *tPlane;
typedef struct{short red,green,blue;} tColor;
typedef struct{short v,h;} tPoint;
typedef struct{short top,left,bottom,right;} tRect;
#endif

/////////////////
/*** 公開関数 ***/
/////////////////

int LoadRsrcFile(void);
char **myGetResource(char *pcName, int resType);
char **myGetFile(char *pcName, int resType);
void mySaveResource(char **handle, int resType, char *name, short id);
char **myNewHandle(int size);
int myDisposeHandle(char **handle);
void myHLock(char **handle);
unsigned long myTickCount(void);
int Halt(void);
int Wait(int ticks);
int myMouse(void);
int myMouseH(void);
int myMouseV(void);
int myWithinRect(int h, int v, tRect *r);
int mySetCursor(char *name);
int myPlaySound(tChannel chan, tSound snd);
int myStopSound(tChannel chan);
int myNewChannel(tChannel *chan);
int myLoadMovie(char *path);
int myPlayMovie(int loop);
int myStopMovie(void);
int myReleaseMovie(void);

#endif /* _MACWRAP_H_ */