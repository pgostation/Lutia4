/* main.c */

#include "mycarbon.h"
#include "main.h"
#include "game.h"



void Initialize(void);	/* function prototypes */
int EventLoop(void);
void MakeWindow(void);
void MakeMenu(void);
void DoEvent(EventRecord *event);
void DoMenuCommand(long menuResult);
void DoAboutBox(void);
void DrawWindow(WindowRef window);
static OSErr QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon);
static void setApplicationPath( const char *path );



static Boolean		gQuitFlag;	/* global */
static WindowRef	myWindow;



int main(int argc, char *argv[])
{
	// chdirする
    if(argc > 0)
		setApplicationPath( argv[0] );
	Initialize();
	MakeWindow();
    MakeMenu();
    
    gQuitFlag = false;
    
	GameMain();
	
    EventLoop();
    ExitToShell();	

    return 0;
}
 
 
 
void Initialize()	/* Initialize the cursor and set up AppleEvent quit handler */
{
    OSErr	err;
        
    InitCursor();
	EnterMovies();

    err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP((AEEventHandlerProcPtr)QuitAppleEventHandler), 0, false );
    if (err != noErr)
        ExitToShell();
}



static OSErr QuitAppleEventHandler( const AppleEvent *appleEvt, AppleEvent* reply, UInt32 refcon )
{
    gQuitFlag =  true;
    
    return noErr;
}



int EventLoop()
{
    Boolean	gotEvent;
    EventRecord	event;
    
    //do
    {
		gotEvent = WaitNextEvent(everyEvent, &event, kSleepTime, nil);
        
        if (gotEvent)
            DoEvent(&event);
        else
            return false;
    }// while (!gQuitFlag);
    
    if(gQuitFlag)
    	ExitToShell();	
    
    return gotEvent;				
}



void MakeWindow(void)	/* Put up a window */
{
    Rect	wRect;
	
    SetRect(&wRect,0,0,0,0); /* left, top, right, bottom */
    
	CreateNewWindow(kModalWindowClass, kWindowNoAttributes, &wRect, &myWindow);
	
    if (myWindow != nil)
	{
        SetPortWindowPort(myWindow);  /* set port to new window */
	}
	else
		ExitToShell();					
}



CGrafPtr getMainPlane(void)
{
	return GetWindowPort(myWindow);
}



void MoveMainWindow(void)
{
	int x,y;
	//Rect ScreenRect = qd.screenBits.bounds;	//メインスクリーンの大きさを得る
	BitMap theScreenBits;
	GetQDGlobalsScreenBits(&theScreenBits); //Carbon
	
	SizeWindow(myWindow, WINDOW_WIDTH, WINDOW_HEIGHT, false);
	
	//スクリーンの中心に表示
	x = (theScreenBits.bounds.right - WINDOW_WIDTH)/2;
	y = (theScreenBits.bounds.bottom - WINDOW_HEIGHT)/2;
	MoveWindow(myWindow, x, y, false);
}



void ShowMainWindow(void)
{
	ShowWindow(myWindow);
}



void MakeMenu()		/* Put up a menu */
{
    Handle	menuBar;
    MenuRef	menu;
    long	response;
    OSErr	err;
	
    menuBar = GetNewMBar(rMenuBar);	/* read menus into menu bar */
    if ( menuBar != nil )
    {
        SetMenuBar(menuBar);	/* install menus */
        
        err = Gestalt(gestaltMenuMgrAttr, &response);
	if ((err == noErr) && (response & gestaltMenuMgrAquaLayoutMask))
        {
            menu = GetMenuHandle( mFile );
            DeleteMenuItem( menu, iQuit );
            DeleteMenuItem( menu, iQuitSeparator );
        }
        
        DrawMenuBar();
    }
    else
    	ExitToShell();
}



void DoEvent(EventRecord *event)
{
    short	part;
    Boolean	hit;
    char	key;
    Rect	tempRect;
    WindowRef	whichWindow;
        
    switch (event->what) 
    {
        case mouseDown:
            part = FindWindow(event->where, &whichWindow);
            switch (part)
            {
                case inMenuBar:  /* process a moused menu command */
                    DoMenuCommand(MenuSelect(event->where));
                    break;
                    
                case inSysWindow:
                    break;
                
                case inContent:
                    if (whichWindow != FrontWindow()) 
                        SelectWindow(whichWindow);
                    break;
                
                case inDrag:	/* pass screenBits.bounds */
                    GetRegionBounds(GetGrayRgn(), &tempRect);
                    DragWindow(whichWindow, event->where, &tempRect);
                    break;
                    
                case inGrow:
                    break;
                    
                case inGoAway:
                    if (TrackGoAway(whichWindow, event->where))
                      DisposeWindow(whichWindow);
                    break;
                    
                case inZoomIn:
                case inZoomOut:
                    hit = TrackBox(whichWindow, event->where, part);
                    if (hit) 
                    {
                        SetPort(GetWindowPort(whichWindow));   // window must be current port
                        EraseRect(GetWindowPortBounds(whichWindow, &tempRect));   // inval/erase because of ZoomWindow bug
                        ZoomWindow(whichWindow, part, true);
                        InvalWindowRect(whichWindow, GetWindowPortBounds(whichWindow, &tempRect));	
                    }
                    break;
                }
			break;
		
		case keyDown:
		case autoKey:
				key = event->message & charCodeMask;
				if (event->modifiers & cmdKey)
					if (event->what == keyDown)
						DoMenuCommand(MenuKey(key));
		case activateEvt:	       /* if you needed to do something special */
				break;
                    
		case updateEvt:
			DrawWindow((WindowRef) event->message);
			break;
                        
		case kHighLevelEvent:
			AEProcessAppleEvent( event );
			break;
		
		case diskEvt:
			break;
	}
}



void DoMenuCommand(long menuResult)
{
    short	menuID;		/* the resource ID of the selected menu */
    short	menuItem;	/* the item number of the selected menu */
	
    menuID = HiWord(menuResult);    /* use macros to get item & menu number */
    menuItem = LoWord(menuResult);
	
    switch (menuID) 
    {
        case mApple:
            switch (menuItem) 
            {
                case iAbout:
                    DoAboutBox();
                    break;
                    
                case iQuit:
                    ExitToShell();
                    break;
				
                default:
                    break;
            }
            break;
        
        case mFile:
            break;
		
        case mEdit:
            break;
    }
    HiliteMenu(0);	/* unhighlight what MenuSelect (or MenuKey) hilited */
}



void DrawWindow(WindowRef window)
{
    Rect		tempRect;
    GrafPtr		curPort;
    
    GetPort(&curPort);
    SetPort(GetWindowPort(window));
    BeginUpdate(window);
	EraseRect(GetWindowPortBounds(window, &tempRect));
	DrawControls(window);
	DrawGrowIcon(window);
    EndUpdate(window);
    SetPort(curPort);
}



void DoAboutBox(void)
{
   (void) Alert(kAboutBox, nil);  // simple alert dialog box
}



/* from kurokoge */
static void setApplicationPath( const char *path )
{
/* MPW's MrCでは素通り */
#ifndef __MRC__
    char current[2048];
    int length = strlen( path );
    int count = 0;

    if( length < sizeof(current) )
        strcpy( current, path );
    else
        return;

    length--;
    while(true)
    {
        if( current[length] == '/' )
        {
            count++;
        }
        
        if( count >= 4 )
        {
            current[length] = '\0';
            break;
        }
        length--;
    }

    chdir( current );
#endif
}

