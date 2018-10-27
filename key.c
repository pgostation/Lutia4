/*
 *  key.c
 *
 *  Copyright (c) 2004 PUGO. All rights reserved.
 *
 */

#include "mycarbon.h"
#include "macWrap.h"
#include "key.h"



/* 専用関数 */
static int myPressKeyMac(unsigned char key);
static int myPushKeyMac(unsigned char key);
static int myPressKey(unsigned long key);
static int myPushKey(unsigned long key);

//静的内部変数
static KeyMap gPushKeyMap;
static KeyMap gKeyMap;

//コンバート
static unsigned long keyName[128] =
{
	'x',	/* 0 */
	'z',	/* 1 */
	'g',	/* 2 */
	'h',	/* 3 */
	'f',	/* 4 */
	'd',	/* 5 */
	's',	/* 6 */
	'a',	/* 7 */
	'r',	/* 8 */
	'e',	/* 9 */
	'w',	/* 10 */
	'q',	/* 11 */
	'b',	/* 12 */
	' ',	/* 13 */
	'v',	/* 14 */
	'c',	/* 15 */
	'%',	/* 16 */
	'^',	/* 17 */
	'$',	/* 18 */
	'#',	/* 19 */
	'@',	/* 20 */
	'!',	/* 21 */
	't',	/* 22 */
	'y',	/* 23 */
	'o',	/* 24 */
	']',	/* 25 */
	')',	/* 26 */
	'*',	/* 27 */
	'-',	/* 28 */
	'&',	/* 29 */
	'(',	/* 30 */
	'+',	/* 31 */
	'"',	/* 32 */
	'j',	/* 33 */
	'l',	/* 34 */
	'retu',	/* 35 */
	'p',	/* 36 */
	'i',	/* 37 */
	'[',	/* 38 */
	'u',	/* 39 */
	'>',	/* 40 */
	'm',	/* 41 */
	'n',	/* 42 */
	'?',	/* 43 */
	'<',	/* 44 */
	'\\',	/* 45 */
	':',	/* 46 */
	'k',	/* 47 */
	'comm',	/* 48 */
	' ',	/* 49 */
	'esc',	/* 50 */
	' ',	/* 51 */
	'dele',	/* 52 */
	'~',	/* 53 */
	'spac',	/* 54 */
	'tab',	/* 55 */
	' ',	/* 56 */
	' ',	/* 57 */
	' ',	/* 58 */
	' ',	/* 59 */
	'cont',	/* 60 */
	'opti',	/* 61 */
	'caps',	/* 62 */
	'shif',	/* 63 */
	'clea',	/* 64 */
	' ',	/* 65 */
	't+',	/* 66 */
	' ',	/* 67 */
	't*',	/* 68 */
	' ',	/* 69 */
	't.',	/* 70 */
	' ',	/* 71 */
	' ',	/* 72 */
	't-',	/* 73 */
	' ',	/* 74 */
	'ente',	/* 75 */
	't/',	/* 76 */
	' ',	/* 77 */
	' ',	/* 78 */
	' ',	/* 79 */
	't5',	/* 80 */
	't4',	/* 81 */
	't3',	/* 82 */
	't2',	/* 83 */
	't1',	/* 84 */
	't0',	/* 85 */
	't=',	/* 86 */
	' ',	/* 87 */
	' ',	/* 88 */
	' ',	/* 89 */
	' ',	/* 90 */
	't9',	/* 91 */
	't8',	/* 92 */
	' ',	/* 93 */
	't7',	/* 94 */
	't6',	/* 95 */
	'f11',	/* 96 */
	' ',	/* 97 */
	'f9',	/* 98 */
	'f8',	/* 99 */
	'f3',	/* 100 */
	'f7',	/* 101 */
	'f6',	/* 102 */
	'f5',	/* 103 */
	'f12',	/* 104 */
	' ',	/* 105 */
	'f10',	/* 106 */
	' ',	/* 107 */
	'f14',	/* 108 */
	' ',	/* 109 */
	'f13',	/* 110 */
	' ',	/* 111 */
	' ',	/* 112 */
	'f4',	/* 113 */
	' ',	/* 114 */
	' ',	/* 115 */
	' ',	/* 116 */
	' ',	/* 117 */
	'f15',	/* 118 */
	' ',	/* 119 */
	'powe',	/* 120 */
	'au',	/* 121 */
	'ad',	/* 122 */
	'ar',	/* 123 */
	'al',	/* 124 */
	'f1',	/* 125 */
	' ',	/* 126 */
	'f2',	/* 127 */
};



static int myPressKeyMac(unsigned char key)
{
	long aKeyMap;

	//KeyMap配列４つのうちどこなの？
	aKeyMap = gKeyMap[key >> 5];
	key &= 0x1f;
	
	//long型変数4バイトのうちのどこなの？
	aKeyMap = *(((char*)&aKeyMap) + (key >> 3));
	key &= 0x07;
	
	//そのキーは押されているの？
	return (aKeyMap & (1 << key)) != 0;
}



static int myPushKeyMac(unsigned char key)
{
	long aKeyMap;

	//KeyMap配列４つのうちどこなの？
	aKeyMap = gPushKeyMap[key >> 5];
	key &= 0x1f;
	
	//long型変数4バイトのうちのどこなの？
	aKeyMap = *(((char*)&aKeyMap) + (key >> 3));
	key &= 0x07;
	
	//そのキーは押されているの？
	return (aKeyMap & (1 << key)) != 0;
}



static int myPressKey(unsigned long key)
{
	int i;
	
	for(i=0; i<128; i++)
	{
	    if(key == keyName[i])
			return myPressKeyMac(i);
	}
	return 0;
}



static int myPushKey(unsigned long key)
{
	int i;
	
	for(i=0; i<128; i++)
	{
	    if(key == keyName[i])
			return myPushKeyMac(i);
	}
	return 0;
}



//キー入力受け付け
int myKeyInput(void)
{
    KeyMap LastKeyMap;
    int i;

    for(i=0; i<4; i++)
	{
        LastKeyMap[i] = gKeyMap[i];
    }
    GetKeys(gKeyMap);
    for(i=0; i<4; i++)
	{
        gPushKeyMap[i] = gKeyMap[i] & ~LastKeyMap[i];
    }
	
	return true;
}




int myPressLeftKey(void)
{
	return myPressKey('a') || myPressKey('al') || myPressKey('t4');
}
int myPushLeftKey(void)
{
	return myPushKey('a') || myPushKey('al') || myPushKey('t4');
}



int myPressRightKey(void)
{
	return myPressKey('d') || myPressKey('ar') || myPressKey('t6');
}
int myPushRightKey(void)
{
	return myPushKey('d') || myPushKey('ar') || myPushKey('t6');
}



int myPressUpKey(void)
{
	return myPressKey('w') || myPressKey('au') || myPressKey('t8');
}
int myPushUpKey(void)
{
	return myPushKey('w') || myPushKey('au') || myPushKey('t8');
}



int myPressDownKey(void)
{
	return myPressKey('s') || myPressKey('x') || myPressKey('au') || myPressKey('t5') || myPressKey('t2');
}
int myPushDownKey(void)
{
	return myPushKey('s') || myPushKey('x') || myPushKey('au') || myPushKey('t5') || myPressKey('t2');
}



int myPressCmdKey(void)
{
	return myPressKey('comm') || myPressKey('f');
}
int myPushCmdKey(void)
{
	return myPushKey('comm') || myPushKey('f');
}



int myPressOptionKey(void)
{
	return myPressKey('opti') || myPressKey('g');
}
int myPushOptionKey(void)
{
	return myPushKey('opti') || myPushKey('g');
}



int myPressShiftKey(void)
{
	return myPressKey('shif') || myPressKey('h');
}
int myPushShiftKey(void)
{
	return myPushKey('shif') || myPushKey('h');
}



int myPressEscKey(void)
{
	return myPressKey('esc') || myPressKey('comm') && myPressKey('p');
}
int myPushEscKey(void)
{
	return myPushKey('esc') || myPushKey('comm') && myPushKey('p');
}