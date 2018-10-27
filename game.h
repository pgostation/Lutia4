/*
 *  game.h
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */
 
#include "macWrap.h"

enum oc_scene
{
	SCENE_LOGO,
	SCENE_TITLE,
	SCENE_MENU,
	SCENE_CONFIG,
	SCENE_SPECIAL,
	SCENE_HOWTO,
	SCENE_GAME,
	SCENE_DEBUG,
	SCENE_QUIT,
};

/////////////////
/*** åˆäJä÷êî ***/
/////////////////
int GameMain(void);