/*
 *  game.c
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#include <stdio.h>
#include "draw.h"
#include "game.h"



/* 専用関数 */
static int GameLogo(void);
static int GameTitle(void);
static int GameMenu(void);
static int GameConfig(void);
static int GameSpecial(void);
static int GameHowTo(void);
static int GameDebug(void);
static int GameGame(void);
static int GameQuit(void);



/* static変数 */
#ifdef __MACOS9__
tPlane os9Plane;
#endif
tPlane mainPlane;
tPlane subPlane;
tPlane tmpPlane;
tPlane tmp2Plane;
tPlane rsrcPlane;
static tPlane localPlane;
static tPlane local2Plane;
tChannel	bgmChan;
tChannel	sndChan;



int GameMain(void)
{
	extern int setBuffer(tPlane plane);
	extern tPlane getMainPlane(void);
	extern void MoveMainWindow(void);
	extern void ShowMainWindow(void);
	int nextscene = SCENE_LOGO;
	tRect r = {0,0,640,480};
	
	LoadRsrcFile();
	mySetCursor("wait");
	
#ifdef __MACOS9__
	os9Plane = getMainPlane();
	myNewPlane(&mainPlane, &r, 0, USE_VRAM);
#else
	mainPlane = getMainPlane();
#endif
	myNewPlane(&subPlane, &r, 0, USE_VRAM); /* VRAM使うときdepthは0 */
	myNewPlane(&tmpPlane, &r, 0, USE_VRAM);
	myNewPlane(&tmp2Plane, &r, 0, USE_VRAM);
	myNewPlane(&rsrcPlane, &r, 0, USE_VRAM);
	myNewPlane(&localPlane, &r, 0, USE_LOCAL);
	myNewPlane(&local2Plane, &r, 0, USE_LOCAL);
	
	myPaint(mainPlane, 0x000000, &r, MODE_COPY, 0);
	
	MoveMainWindow();
	ShowMainWindow();
	
	myNewChannel(&bgmChan);
	myNewChannel(&sndChan);
	
	Wait(15);
	
	while(1)
	{
		switch(nextscene)
		{
			case SCENE_LOGO:
				nextscene = GameLogo();
				break;
			
			case SCENE_TITLE:
				nextscene = GameTitle();
				break;
			
			case SCENE_MENU:
				nextscene = GameMenu();
				break;
			
			case SCENE_CONFIG:
				nextscene = GameConfig();
				break;
			
			case SCENE_SPECIAL:
				nextscene = GameSpecial();
				break;
			
			case SCENE_HOWTO:
				nextscene = GameHowTo();
				break;
			
			case SCENE_DEBUG:
				nextscene = GameDebug();
				break;
			
			case SCENE_GAME:
				nextscene = GameGame();
				break;
			
			case SCENE_QUIT:
				nextscene = GameQuit();
				return 0;
		}
	}
	
	return 0;
}



static int GameLogo(void)
{
	tRect   r = {0,0,480,480};
	
	myPaint(mainPlane, 0x000000, &r, MODE_COPY, 0);
	Wait(0);
	
	return SCENE_TITLE;
}



static int GameTitle(void)
{
	return SCENE_MENU;
}



static int GameMenu(void)
{
	tPoint p = {64,64};

	myLoadCicn(mainPlane, "150", NULL, &p);
	Wait(60);
	
	return SCENE_TITLE;
}



static int GameConfig(void)
{
	return SCENE_MENU;
}



static int GameSpecial(void)
{
	return SCENE_TITLE;
}



static int GameHowTo(void)
{
	return SCENE_TITLE;
}



static int GameDebug(void)
{
	return SCENE_TITLE;
}



static int GameGame(void)
{
	//ゲームメイン関数をここに
	
	return SCENE_TITLE;
}



static int GameQuit(void)
{
	return -1;
}
