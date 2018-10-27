/*
 *  mycarbon.h
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#ifndef _MYCARBON_H_
#define _MYCARBON_H_

#ifdef __MRC__
	/* MPW's MrC */
	#include <Carbon.h>
	#include <QuickTime.h>
#else
	/* Other(Xcode,ProjectBuilder) */
	#include <Carbon/Carbon.h>
	#include <QuickTime/QuickTime.h>
#endif

#endif /* _MYCARBON_H_ */