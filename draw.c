/*
 *  draw.c
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#define _DRAW_C_

#include "mycarbon.h"
#include "draw.h"

#define MIN(a,b)	(a)<(b)? (a):(b)
#define MAX(a,b)	(a)>(b)? (a):(b)



int myNewPlane(tPlane* pPlane, tRect *r, int depth, int opt)
{
    OSErr	err;
    
    /* GWorld作成 */
	if(opt == USE_VRAM)
		err = NewGWorld(pPlane, depth, r, NULL, NULL, useDistantHdwrMem);
    else
		err = NewGWorld(pPlane, depth, r, NULL, NULL, useLocalHdwrMem);
	if(err != noErr)
    {
        return false;
    }

    /* GWorldをクリア */
    {
        CGrafPtr 	saveGrafPort;
        GDHandle 	saveGDHandle;
        
        GetGWorld(&saveGrafPort, &saveGDHandle);
        SetGWorld(*pPlane, nil);
        EraseRect(r);
        SetGWorld(saveGrafPort, saveGDHandle);
    }
    
    return true;
}



int myDeletePlane(tPlane plane)
{
	if(plane == NULL)
		return false;
	
	DisposeGWorld(plane);
	
	return true;
}



int myPaint(tPlane plane, int col, tRect *r, short mode, int opt)
{
	tColor		saveColor;
	tColor		color;
	CGrafPtr 	saveGrafPort;
	GDHandle 	saveGDHandle;

	color.red   = 256*((col & 0x00ff0000) >> 16);
	color.green = 256*((col & 0x0000ff00) >>  8);
	color.blue  = 256*((col & 0x000000ff) >>  0);
	
	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(plane, nil);
	GetForeColor(&saveColor);
	RGBForeColor(&color);
	
	switch(mode)
	{
		case MODE_BLEND:
		case MODE_ADDPIN:
		case MODE_SUBPIN:
			color.red   = opt*65535/100;
			color.green = opt*65535/100;
			color.blue  = opt*65535/100;
			OpColor(&color);
			break;
	}
	PenMode(mode);
	
	PaintRect(r);
	
	PenMode(srcCopy);
	RGBForeColor(&saveColor);
	SetGWorld(saveGrafPort, saveGDHandle);
	
	return true;
}



int myPictFrame(tRect *r, char *name)
{
	int			num;
	PicHandle	pict = NULL;
	Str255		pname;
	
	c2pstrcpy(pname, name);
	pict = (PicHandle)GetNamedResource('PICT', pname);
	if(pict == NULL)
	{
		num = atoi(name);
		if(num > 32767)
			return false;
		pict = (PicHandle)GetResource('PICT', num);
		if(pict == NULL)
			return false;
	}
	
	*r = (**pict).picFrame;
	OffsetRect(r, -r->left, -r->top);

	ReleaseResource((Handle)pict);
	
	return true;
}



int myLoadPict(tPlane plane, char *name, tRect *r, tPoint *p)
{
	int			num;
	PicHandle	pict = NULL;
	Str255		pname;
	tRect		rect;
	
	c2pstrcpy(pname, name);
	pict = (PicHandle)GetNamedResource('PICT', pname);
	if(pict == NULL)
	{
		num = atoi(name);
		if(num > 32767)
			return false;
		pict = (PicHandle)GetResource('PICT', num);
		if(pict == NULL)
			return false;
	}
	
	/* PICTのRectを抽出 */
	if(r)
	{
		rect = *r;
	}
	else if(p)
	{
		rect = (**pict).picFrame;
		OffsetRect(&rect, -rect.left, -rect.top);			//PICTの左上を(0,0)に
		OffsetRect(&rect, -rect.right/2, -rect.bottom/2);   //(0,0)をPICTの中心に
		OffsetRect(&rect, p->h, p->v);						//そこから中心をずらす
	}
	else return false;
	
	{
		CGrafPtr 	saveGrafPort;
		GDHandle 	saveGDHandle;

		HLock((Handle)pict);
		
		//GWorldにPICTを描画
		GetGWorld(&saveGrafPort, &saveGDHandle);
		SetGWorld(plane, nil);
		DrawPicture(pict, &rect);
		SetGWorld(saveGrafPort, saveGDHandle);
		
		HUnlock((Handle)pict);
		ReleaseResource((Handle)pict);
	}
	
	return true;
}



int myLoadCicn(tPlane plane, char *name, tRect *r, tPoint *p)
{
    CIconHandle cicn = NULL;
	Str255		pname;
	short		num;
	unsigned long resType;
	tRect		rect;
	
    num = atoi(name);
    if(num > 0)
    {
        cicn = GetCIcon(num);
    }
	
	if(!cicn)
	{
		SetResLoad(false);
		c2pstrcpy(pname, name);
		cicn = (CIconHandle)GetNamedResource('cicn', pname);
		SetResLoad(true);
		if(!cicn)
		{
			return false;
		}
		GetResInfo((Handle)cicn, &num, &resType, pname);
		cicn = GetCIcon(num);
	}
	
	if(r)
	{
		rect = *r;
	}
	else if(p)
	{
		rect = (*cicn)->iconPMap.bounds;
		OffsetRect(&rect, -rect.left, -rect.top);			//PICTの左上を(0,0)に
		OffsetRect(&rect, -rect.right/2, -rect.bottom/2);   //(0,0)をPICTの中心に
		OffsetRect(&rect, p->h, p->v);						//そこから中心をずらす
	}
	else return false;
	
	{
		CGrafPtr 	saveGrafPort;
		GDHandle 	saveGDHandle;

		GetGWorld(&saveGrafPort, &saveGDHandle);
		LockPixels(GetGWorldPixMap(plane));
		SetGWorld(plane, nil);
		
		HLock((Handle)cicn);
		PlotCIcon(&rect,cicn);
		HUnlock((Handle)cicn);

		DisposeCIcon(cicn);
		
		UnlockPixels(GetGWorldPixMap(plane));
		SetGWorld(saveGrafPort, saveGDHandle);
	}
	
	return true;
}



int myCopyBits(tPlane src, tPlane dst, tRect *sr, tRect *dr, int mode, int opt)
{
	tColor		color;
	CGrafPtr 	saveGrafPort;
	GDHandle 	saveGDHandle;
	
	color.red   = 256*((opt & 0x00ff0000) >> 16);
	color.green = 256*((opt & 0x0000ff00) >>  8);
	color.blue  = 256*((opt & 0x000000ff) >>  0);

	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(dst, nil);
	
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

	SimpleCopyBits(src, dst, sr, dr, mode);
	
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



int myDrawPlane(tPlane src, tPlane dst, tRect *dr, tPoint *pt, int mode, int opt)
{
	tColor		color;
	CGrafPtr 	saveGrafPort;
	GDHandle 	saveGDHandle;
	tRect		srcr, dstr;
	
	color.red   = 256*((opt & 0x00ff0000) >> 16);
	color.green = 256*((opt & 0x0000ff00) >>  8);
	color.blue  = 256*((opt & 0x000000ff) >>  0);

	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(dst, nil);
	
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
		if(pt == NULL)
		{
			getPlaneRect(&dstr, dst);
		}
		else
		{
			getPlaneRect(&dstr, src);
			OffsetRect(&dstr, -dstr.left, -dstr.top);
			OffsetRect(&dstr, -dstr.right/2, -dstr.bottom/2);
			OffsetRect(&dstr, pt->h, pt->v);
		}
	}
	else
	{
		dstr = *dr;
	}

	SimpleCopyBits(src, dst, getPlaneRect(&srcr, src), &dstr, mode);
	
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



int SimpleCopyBits(tPlane src, tPlane dst, tRect *sr, tRect *dr, int mode)
{
	CopyBits((BitMap *)*GetGWorldPixMap(src), (BitMap *)*GetGWorldPixMap(dst),
		sr, dr, mode, NULL
	);
	
	return true;
}



#if 0
/* FONTのリソースへの組み込み方がよく分からない */
int myDrawString(tPlane plane, char *string, int col, char *name, int size, int face, tPoint *pt, int mode, int opt)
{
	Str255		pname;
	tColor		color, saveColor;
	short		font;
	FontInfo 	fInfo;
	CGrafPtr 	saveGrafPort;
	GDHandle 	saveGDHandle;
	
	//mode抽出
	if(mode < 8)
	{
		mode += 8;
	}
	
	//HyperCardのfont設定を取り出す
	c2pstrcpy(pname, name);
	GetFNum(pname,&font);
	
	//ポートを設定
	GetGWorld(&saveGrafPort, &saveGDHandle);
	SetGWorld(plane, nil);
	
	//OpColor抽出
	switch(mode)
	{
		case MODE_BLEND:
		case MODE_ADDPIN:
		case MODE_SUBPIN:
			color.red   = opt*65535/100;
			color.green = opt*65535/100;
			color.blue  = opt*65535/100;
			OpColor(&color);
			break;
	}
	
	color.red   = 256*((col & 0x00ff0000) >> 16);
	color.green = 256*((col & 0x0000ff00) >>  8);
	color.blue  = 256*((col & 0x000000ff) >>  0);
	
	GetForeColor(&saveColor);
	RGBForeColor(&color);
	TextFont(font);
	TextSize(size);
	TextMode(mode);
	TextFace(face);
	
	GetFontInfo(&fInfo);
	MoveTo(pt->h, pt->v + fInfo.ascent);
	c2pstrcpy(pname, string);
	DrawString(pname);
	
	RGBForeColor(&saveColor);
	SetGWorld(saveGrafPort, saveGDHandle);
	
	return true;
}
#endif



int myClipRect(tRect *r)
{
	ClipRect(r);
	
	return true;
}



int jntMorph(tRect *srcRect, tRect *dstRect,
	tPlane backBuf, tPlane srcBuf, tPlane dstBuf, tPlane tmpBuf, tPlane mainBuf,
	int srcMode, int dstMode, int srcAlpha, int dstAlpha, int time, int option)
{
	unsigned long   t;
	tPoint			srcLoc, dstLoc;
	int				srcWidth, srcHeight, dstWidth, dstHeight;
	
	srcLoc.h = (srcRect->left + srcRect->right)/2;
	srcLoc.v = (srcRect->top + srcRect->bottom)/2;
	dstLoc.h = (dstRect->left + dstRect->right)/2;
	dstLoc.v = (dstRect->top + dstRect->bottom)/2;
	srcWidth = srcRect->right - srcRect->left; 
	srcHeight = srcRect->bottom - srcRect->top; 
	dstWidth = dstRect->right - dstRect->left;
	dstHeight = dstRect->bottom - dstRect->top;
	t = myTickCount();

	while(1)
	{
		double  s, it;
		tPoint  morphLoc;
		double  srcSize, dstSize, size;
		double  srcDiff, dstDiff, diff;
		double  v1, v2, v, h;
		tRect   morphRect, r1, r2;
		
		switch(option)
		{
			case 0:
				s = MIN(1.0,(myTickCount() - t)/(double)time);
				break;
			case 1:		/* up */
				it = MIN(1.0,(myTickCount() - t)/(double)time);
				s = it*it;
				break;
			case -1:	/* down */
				s = MIN(1.0,sqrt(sqrt((myTickCount() - t)/(double)time)));
				break;
			default:
				s = 0.0;
		}
		
		//中心位置は直線的に移動
		morphLoc.h = srcLoc.h * (1.0-s) + dstLoc.h * s;
		morphLoc.v = srcLoc.v * (1.0-s) + dstLoc.v * s;
		//面積も直線的に変化する
		srcSize = srcWidth * srcHeight;
		dstSize = dstWidth * dstHeight;
		size = srcSize * (1.0-s) + dstSize * s;
		//縦横の差も直線的に変化する
		srcDiff = srcWidth - srcHeight;
		dstDiff = dstWidth - dstHeight;
		diff = srcDiff * (1.0-s) + dstDiff * s;
		//そこから辺の比を求める
		// v = max((-d+sqrt(d*d+4*s))/2 , (-d-sqrt(d*d+4*s))/2)
		v1 = (-diff + sqrt(diff*diff+4.0*size))/2.0;
		v2 = (-diff - sqrt(diff*diff+4.0*size))/2.0;
		v = MAX(v1,v2);
		h = size / v;
		morphRect.left = morphLoc.h - h/2.0;
		morphRect.top = morphLoc.v - v/2.0;
		morphRect.right = morphLoc.h + h/2.0;
		morphRect.bottom = morphLoc.v + v/2.0;

		myCopyBits(backBuf, tmpBuf, getPlaneRect(&r1, backBuf), getPlaneRect(&r2, tmpBuf), MODE_COPY, 0x00);
		if(srcMode == MODE_COPY)
			myCopyBits(srcBuf, tmpBuf, getPlaneRect(&r1, srcBuf), &morphRect, MODE_BLEND, 100*(1.0-s));
		else
			myCopyBits(srcBuf, tmpBuf, getPlaneRect(&r1, srcBuf), &morphRect, srcMode, srcAlpha);
		if(dstMode == MODE_COPY)
			myCopyBits(dstBuf, tmpBuf, getPlaneRect(&r1, dstBuf), &morphRect, MODE_BLEND, 100*s);
		else
			myCopyBits(dstBuf, tmpBuf, getPlaneRect(&r1, dstBuf), &morphRect, dstMode, dstAlpha);
		myCopyBits(tmpBuf, mainBuf, getPlaneRect(&r1, tmpBuf), getPlaneRect(&r2, mainBuf), MODE_COPY, 0x00);
		Halt();
		if(s == 1)
			break;
	}
  
	return true;
}



tRect *getPlaneRect(tRect *r, tPlane plane)
{
	GetPortBounds((CGrafPtr)plane, r);

	return r;
}

