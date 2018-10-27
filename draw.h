/*
 *  draw.h
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#ifndef _DRAW_H_
#define _DRAW_H_

#include "macWrap.h"
#include "sprite.h"

enum oc_drawmode
{
	MODE_COPY = 0,
	MODE_OR,
	MODE_XOR,
	MODE_BIC,
	MODE_NOTCOPY,
	MODE_NOTOR,
	MODE_NOTXOR,
	MODE_NOTBIC,
	MODE_BLEND = 32,
	MODE_ADDPIN,
	MODE_ADDOVER,
	MODE_SUBPIN,
	MODE_TRANSPARENT,
	MODE_ADMAX,
	MODE_SUBOVER,
	MODE_ADMIN,
	MODE_GRAYISHTEXTOR = 49,
	MODE_HILITE,
	MODE_DITHERCOPY = 64,
};

enum oc_facebit
{
	FACE_PLAIN = 0,
	FACE_ITALIC,
	FACE_BOLD,
	FACE_UNDERLINE,
	FACE_OUTLINE,
	FACE_SHADOW,
	FACE_CONDENSE,
	FACE_EXTEND,
};

enum oc_planeoption
{
	USE_LOCAL,
	USE_VRAM,
};

/////////////////
/*** åˆäJä÷êî ***/
/////////////////
int myNewPlane(tPlane* pPlane, tRect *r, int depth, int opt);
int myDeletePlane(tPlane plane);
int myPaint(tPlane plane, int col, tRect *r, short mode, int opt);
int myPictFrame(tRect *r, char *name);
int myLoadPict(tPlane plane, char *name, tRect *r, tPoint *p);
int myLoadCicn(tPlane plane, char *name, tRect *r, tPoint *p);
int myCopyBits(tPlane src, tPlane dst, tRect *sr, tRect *dr, int mode, int opt);
int SimpleCopyBits(tPlane src, tPlane dst, tRect *sr, tRect *dr, int mode);
int myDrawPlane(tPlane src, tPlane dst, tRect *dr, tPoint *pt, int mode, int opt);
//int myDrawString(tPlane plane, char *string, int col, char *name, int size, int face, tPoint *pt, int mode, int opt);
int myClipRect(tRect *r);
int jntMorph(tRect *srcRect, tRect *dstRect,
	tPlane backBuf, tPlane srcBuf, tPlane dstBuf, tPlane tmpBuf, tPlane mainBuf,
	int srcMode, int dstMode, int srcAlpha, int dstAlpha, int time, int option);
tRect *getPlaneRect(tRect *r, tPlane plane);

#endif /* _DRAW_H_ */