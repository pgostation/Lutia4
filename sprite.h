/*
 *  sprite.h
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "macWrap.h"
#include "draw.h"



struct sprite
{
	tPlane  plane;
	tPlane  mask;
	tRect   rect;
	int		oh;
	int		ov;
};

typedef struct
{
	struct sprite   sprite[256];
} tSprite;



int mySetSprite(tSprite *spr, tPlane plane, tRect *r, char c);
int myDrawSprite(tPlane plane, tSprite *spr, unsigned char c, tRect *r, tPoint *pt, int mode, int opt);
int myStringSprite(tPlane plane, tSprite *spr, char *string, int size, tRect *r, int mode, int opt);

#endif /* _SPRITE_H_ */