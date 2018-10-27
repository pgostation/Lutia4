/*
 *  sprite.c
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#include "mycarbon.h"
#include "sprite.h"



int mySetSprite(tSprite *spr, tPlane plane, tRect *r, char c)
{
	unsigned char uc = (unsigned char)c;
	
	if(!spr) return false;
	spr->sprite[uc].plane = plane;
	spr->sprite[uc].rect = *r;
	
	return true;
}



int myDrawSprite(tPlane plane, tSprite *spr, unsigned char uc, tRect *r, tPoint *pt, int mode, int opt)
{
	tPlane		src;
	tColor		color;
	tRect		*srcr, dstr;
	CGrafPtr	saveGrafPort;
	GDHandle	saveGDHandle;
	
	if(!spr) return false;
	
	color.red   = 256*((opt & 0x00ff0000) >> 16);
	color.green = 256*((opt & 0x0000ff00) >>  8);
	color.blue  = 256*((opt & 0x000000ff) >>  0);
	
	//ポートを設定
	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(plane, nil);
	
	//OpColor抽出
	switch(mode)
	{
		case MODE_TRANSPARENT:
			RGBBackColor(&color);
			break;
		case MODE_BLEND:
		case MODE_ADDPIN:
		case MODE_SUBPIN:
			color.red   = opt*65535/100;
			color.green = opt*65535/100;
			color.blue  = opt*65535/100;
			OpColor(&color);
			break;
		default:
			RGBForeColor(&color);
			break;
	}
	
	src = spr->sprite[(int)uc].plane;
	srcr = &spr->sprite[(int)uc].rect;
	
	if(r == NULL)
	{
		dstr = *srcr;
		OffsetRect(&dstr, -dstr.left, -dstr.top);
		OffsetRect(&dstr, -dstr.right/2, -dstr.bottom/2);
		if(pt != NULL)
		{
			OffsetRect(&dstr, pt->h, pt->v);
		}
		OffsetRect(&dstr, spr->sprite[(int)uc].oh, spr->sprite[(int)uc].ov);
	}
	else
	{
		dstr = *r;
	}

	SimpleCopyBits(src, plane, &spr->sprite[(int)uc].rect, &dstr, mode );

	color.red = 0;
	color.green = 0;
	color.blue = 0;
	RGBForeColor(&color);
	color.red = 65535;
	color.green = 65535;
	color.blue = 65535;
	RGBBackColor(&color);
	
	SetGWorld(saveGrafPort, saveGDHandle);
	
	return true;
}



int myStringSprite(tPlane plane, tSprite *spr, char *string, int size, tRect *dr, int mode, int opt)
{
	tPlane		src;
	tColor		color;
	tRect		*srcr, dstr, dstr2;
	CGrafPtr	saveGrafPort;
	GDHandle	saveGDHandle;
	
	if(!spr) return false;
	if(*string == '\0') return true;
	
	color.red   = 256*((opt & 0x00ff0000) >> 16);
	color.green = 256*((opt & 0x0000ff00) >>  8);
	color.blue  = 256*((opt & 0x000000ff) >>  0);
	
	//ポートを設定
	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(plane, nil);
	
	//OpColor抽出
	switch(mode)
	{
		case MODE_TRANSPARENT:
			RGBBackColor(&color);
			break;
		case MODE_BLEND:
		case MODE_ADDPIN:
		case MODE_SUBPIN:
			color.red   = opt*65535/100;
			color.green = opt*65535/100;
			color.blue  = opt*65535/100;
			OpColor(&color);
			break;
		default:
			RGBForeColor(&color);
			break;
	}
	
	if(dr == NULL)
	{
		dstr.left = 0;
		dstr.top = 0;
	}
	else
	{
		dstr.left = dr->left;
		dstr.top = dr->top;
		myClipRect(dr);
	}
	
	while(*string != '\0')
	{
		src = spr->sprite[(int)*string].plane;
		srcr = &spr->sprite[(int)*string].rect;
		
		dstr.right = dstr.left + (srcr->right - srcr->left);
		dstr.bottom = dstr.top + (srcr->bottom - srcr->top);

		dstr2 = dstr;
		OffsetRect(&dstr2, spr->sprite[(int)*string].oh, spr->sprite[(int)*string].ov);

		SimpleCopyBits(src, plane, &spr->sprite[(int)*string].rect, &dstr2, mode );
		
		if(*string == '\n')
		{
			dstr.left = dr->left;
			dstr.top = dstr.bottom;
		}
		else
		{
			dstr.left = dstr.right;
		}
		string++;
	}
	
	color.red = 0;
	color.green = 0;
	color.blue = 0;
	RGBForeColor(&color);
	color.red = 65535;
	color.green = 65535;
	color.blue = 65535;
	RGBBackColor(&color);
	
	myClipRect(getPlaneRect(&dstr, plane));
	
	SetGWorld(saveGrafPort, saveGDHandle);
	
	return true;
}



