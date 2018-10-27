/*
 *  macWrap.c
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#define _MACWRAP_C_

#include <stdio.h>
#include "mycarbon.h"
#include "macWrap.h"



static short gRefNum = 0;
static int PlayForward(void);



OSErr GetApplicationPackageFSSpecFromBundle( FSSpecPtr theFSSpecPtr )
{
    OSErr err = fnfErr;
    CFBundleRef myAppsBundle;
    FSRef myBundleRef;
    CFURLRef myBundleURL;
    Boolean ok;
    
    myAppsBundle = CFBundleGetMainBundle();
    if( myAppsBundle == NULL )
        return err;

    myBundleURL = CFBundleCopyBundleURL( myAppsBundle );
    if( myBundleURL == NULL )
        return err;

    ok = CFURLGetFSRef( myBundleURL, &myBundleRef );

    CFRelease( myBundleURL );
    if( !ok )
        return err;

    return FSGetCatalogInfo( &myBundleRef, kFSCatInfoNone,
                             NULL, NULL, theFSSpecPtr, NULL);
}



int LoadRsrcFile(void)
{
	srand((unsigned) time(NULL));	/* 乱数の種 */
	
#ifndef __MRC__
    /* リソースファイルを使う実験 */
    {
        FSSpec LoadFile, RsrcFile;
        char rfilename[512];
        OSErr err;
        
		GetApplicationPackageFSSpecFromBundle(&LoadFile);
        FSSpecToNativePathName(&LoadFile,rfilename,512-12,0);
        strcat(rfilename,"/../main.rsrc");
        err = NativePathNameToFSSpec(rfilename, &RsrcFile,0);
        gRefNum = FSpOpenResFile(&RsrcFile,fsCurPerm);
		if(err == noErr)
        {
            UseResFile(gRefNum);
			return true;
        }
    }
#endif
	
	return false;
}



char **myGetResource(char *pcName, int resType)
{
    Str255 tmpStr;
    Handle rsrc;
    
    /* 名前で検索 */
    c2pstrcpy(tmpStr, pcName);
    if(rsrc = GetNamedResource(resType, tmpStr))
    {
        return (char **)rsrc;
    }
    
    /* 番号で検索 */
    if(rsrc = GetResource(resType, atoi(pcName)))
    {
        return (char **)rsrc;
    }
    
    return NULL;
}



char **myGetFile(char *pcName, int resType)
{
    FILE *fp;
    char buf[1024];
    char **pData = NULL;
    
    if( !(fp = fopen(pcName, "r")))
    {
        return NULL;
    }

    pData = NewHandleClear(2048);
    
    while( fgets(buf, 1024, fp ) )
    {
        if(strlen(*pData) + strlen(buf) >= 2048)
        {
            break;
        }
        strcat(*pData, buf);
    }
    fclose(fp);

    return pData;
}



void mySaveResource(char **handle, int resType, char *name, short id)
{
	Str255  pname;
	char	**reshandle;
	
	reshandle = GetResource(resType, id);
	if(reshandle)
		RemoveResource(reshandle);
	
	c2pstrcpy(pname, name);
    HLock(handle);
    AddResource(handle, resType, id, pname);
    SetResAttrs(handle, resPurgeable);
    ChangedResource(handle);
    WriteResource(handle);
    HUnlock(handle);
}



char **myNewHandle(int size)
{
	return NewHandleClear(size);
}



int myDisposeHandle(char **handle)
{
	DisposeHandle(handle);
	
	return true;
}



void myHLock(char **handle)
{
	HLock(handle);
}



/* システムの起動から現在までの経過時間を1tick=1/60secとして求める */
unsigned long myTickCount(void)
{
	return TickCount();
}



int Halt(void)
{
	extern int EventLoop(void);
	extern tPlane getMainPlane(void);

#ifdef __MACOS9__
	extern tPlane mainPlane, os9Plane;
	tRect r1, r2;
	SimpleCopyBits(mainPlane, os9Plane, getPlaneRect(&r1, mainPlane), getPlaneRect(&r2, os9Plane), srcCopy, NULL);
#else
	QDFlushPortBuffer(getMainPlane(), NULL);
#endif
	
	return true;
}



int Wait(int ticks)
{
	extern int EventLoop(void);
	extern tPlane getMainPlane(void);
	static unsigned long lastTicks = 0;
	int waitTicks;

	if(lastTicks == 0)
		lastTicks = myTickCount();

#ifdef __MACOS9__
	{
		extern tPlane mainPlane, os9Plane;
		tRect r1, r2;
		SimpleCopyBits(mainPlane, os9Plane, getPlaneRect(&r1, mainPlane), getPlaneRect(&r2, os9Plane), srcCopy, NULL);
	}
#else
	QDFlushPortBuffer(getMainPlane(), NULL);
#endif
		
	PlayForward();
	
    /* イベントループをまわす */
    while(1)
    {
        EventLoop();
		if(ticks <= myTickCount() - lastTicks)
			break;    /* 時間が経過したら帰る */
    }
	
	waitTicks = myTickCount() - lastTicks;
	lastTicks = myTickCount();
	
	return waitTicks;
}



int myMouse(void)
{
	return Button();
}



int myMouseH(void)
{
	Point MouseLoc;
	GetMouse(&MouseLoc);
	
	return MouseLoc.h;
}



int myMouseV(void)
{
	Point MouseLoc;
	GetMouse(&MouseLoc);
	
	return MouseLoc.v;
}



int myWithinRect(int h, int v, tRect *r)
{
	if(h < r->left) return false;
	if(h >= r->right) return false;
	if(v < r->top) return false;
	if(v >= r->bottom) return false;
	
	return true;
}



int mySetCursor(char *name)
{
	CCrsrHandle cursor = NULL;
	short		num;
	Str255		pname;
	unsigned long resType;
	
    num = atoi(name);
    if(num > 0)
    {
        cursor = GetCCursor(num);
    }
	
	if(!cursor)
	{
		SetResLoad(false);
		c2pstrcpy(pname, name);
		cursor = (CCrsrHandle)GetNamedResource('crsr', pname);
		SetResLoad(true);
		if(!cursor)
		{
			return false;
		}
		GetResInfo((Handle)cursor, &num, &resType, pname);
		cursor = GetCCursor(num);
	}
	
	SetCCursor(cursor);
	DisposeHandle((Handle)cursor);
	
	return true;
}



int myPlaySound(tChannel chan, tSound snd)
{
    //SndCommand		sndcmd;

    if(!snd || !chan)
        return false;
	
    HLock((Handle)snd);
    if(noErr != SndPlay(chan,snd,true))
    {
        //sndcmd.cmd = callBackCmd;
        //aComm.param2 = SetCurrentA5();
        //if(noErr == SndDoCommand(chan, &sndcmd, true))
        //{
        //    return;
        //}
		return false;
    }
    //SndDisposeChannel(chan,true);
    //HUnlock((Handle)snd);
	
	return true;
}



//チャンネル取得関数
int myNewChannel(tChannel *chan)
{
	if(noErr != SndNewChannel(chan, 0, initStereo, NULL))
		return false;
	
	return true;
}



//チャンネル取得関数
int myStopSound(tChannel chan)
{
    SndCommand	SndCmd;
	
	SndCmd.cmd = quietCmd;
	SndCmd.param1 = 0;
	SndCmd.param2 = 0;
	
	SndDoImmediate(chan, &SndCmd);
	
	SndCmd.cmd = flushCmd;
	SndCmd.param1 = 0;
	SndCmd.param2 = 0;
	
	SndDoImmediate(chan, &SndCmd);
	
	return true;
}



static Movie	theMovie = NULL;
static int		loopMovie = false;

int myLoadMovie(char *path)
{
	FSSpec  LoadFile, MovFile;
	char	filename[512];
	short   RefNum;
	OSErr   err;
#ifdef __MRC__
	long	DirID;
	Str255	FilePath;
	
	err = HGetVol(0L,&RefNum,&DirID);
	if(err != noErr) return;
	
	c2pstrcpy(FilePath,path);
	err = FSMakeFSSpec(RefNum,DirID,FilePath,&MovFile);
	if(err != noErr) return;
#else
	GetApplicationPackageFSSpecFromBundle(&LoadFile);
	FSSpecToNativePathName(&LoadFile,filename,512-12,0);
	strcat(filename,path);
	if(noErr != NativePathNameToFSSpec(filename, &MovFile,0))
	{
		return false;
	}
	RefNum = FSpOpenResFile(&MovFile,fsCurPerm);
	if(RefNum == 0)
	{
		return false;
	}
#endif
	
	//古いMovieのデータの破棄
	if( ! IsMovieDone( theMovie ) )
	{
		DisposeMovie( theMovie );
	}
	else if( theMovie != NULL )
	{
		DisposeMovie( theMovie );
	}
	//Movieファイル取得
	OpenMovieFile( &MovFile, &RefNum, fsRdPerm );
	err = NewMovieFromFile( &theMovie, RefNum, NULL, NULL, newMovieActive, NULL );
	CloseMovieFile( RefNum );
	
	//Movieの開始場所を先頭に設定
	GoToBeginningOfMovie( theMovie );
	
	return true;
}



int myPlayMovie(int loop)
{
	if( theMovie == NULL)
		return false;
	
	loopMovie = loop;
	StartMovie( theMovie );
	
	return true;
}



int myStopMovie(void)
{
	if( theMovie == NULL)
		return false;
	
	StopMovie( theMovie );
	
	return true;
}



int myReleaseMovie(void)
{
	if( theMovie == NULL)
		return false;
	
	DisposeMovie( theMovie );
	
	return true;
}



int PlayForward(void)
{
	if( theMovie == NULL)
		return false;
	
	if( loopMovie && IsMovieDone(theMovie) )
	{
		//Movieの開始場所を先頭に設定
		GoToBeginningOfMovie( theMovie );
	}
	//Movieの更新
	MoviesTask( theMovie, 1 );
	
	return true;
}