/*
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: d3dmain.cpp
 *
 *  Each of the Direct3D samples must be linked with this file.  It contains
 *  the code which allows them to run in the Windows environment.
 *
 *  A window is created using d3dmain.res which allows the user to select the
 *  Direct3D driver to use and change the render options.  The D3DApp
 *  collection of functions is used to initialize DirectDraw, Direct3D and
 *  keep surfaces and D3D devices available for rendering.
 *
 *  Frame rate and a screen mode information buffer is Blt'ed to the screen
 *  by functions in stats.cpp.
 *
 *  Each sample is executed through the functions: InitScene, InitView,
 *  RenderScene, ReleaseView, ReleaseScene and OverrideDefaults, as described
 *  in d3ddemo.h.  Samples can also read mouse and keyboard input via
 *  SetMouseCallback and SetKeyboardCallback.
 */

#define INITGUID

#include "d3dmain.h"
#include "d3dappi.h"
#include "sound.h"
#include "editor.h"
#include "toolbar.h"
#include "world.h"
#include "bitmap.h"
#include "input.h"
#include "info.hpp"
#include "texture.h"
#include <winbase.h>
#include <dinput.h>
#include <dsound.h>

// GLOBAL VARIABLES

extern int PrintMsgY;
 
D3DAppInfo* d3dapp;	    /* Pointer to read only collection of DD and D3D
						   objects maintained by D3DApp */
d3dmainglobals myglobs;	/* collection of global variables */

BOOL(*MouseHandler)(UINT, WPARAM, LPARAM);    /* sample's function which traps
												 mouse input */
BOOL(*KeyboardHandler)(UINT, WPARAM, LPARAM); /* sample's functions which traps */
          
WORLD world;                                      
EDITOR ed;
CAMERA cam;
setupinfo_ptr setupinfo;	

int keycode;
int car_gear=1;
int editor_insert_mode=0;
int MouseX,MouseY;
int xPos,yPos;
int object_rot_angle =0;
int old_xPos, old_yPos;
int toggle_delay = 0;
int current_mode;
int current_screen_status = FULLSCREEN_ONLY; // default, in case rrsetup.ini 
											 // bugs out

int cell_x, cell_z;

BOOL update_player_position_flag;
BOOL ScrollRangeFlag = FALSE;
HWND hwndScrollBar;
SCROLLINFO lpsi;
DWORD last_error;

LPDIRECTDRAWSURFACE bk;
float temp_view_angle;



/*
 *  INTERNAL FUNCTION PROTOTYPES
 */
static BOOL AppInit(HINSTANCE hInstance, LPSTR lpCmdLine);
static BOOL CreateD3DApp(LPSTR lpCmdLine);
static BOOL BeforeDeviceDestroyed(LPVOID lpContext);
static BOOL AfterDeviceCreated(int w, int h, LPDIRECT3DVIEWPORT2* lpViewport,
			       LPVOID lpContext);
void CleanUpAndPostQuit(void);
static void InitGlobals(void);
static BOOL AppPause(BOOL f);
void ReportD3DAppError(void);
static BOOL RenderLoop(void);
static BOOL RestoreSurfaces();
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam,
			   LPARAM lParam );


/****************************************************************************/
/*                            WinMain                                       */
/****************************************************************************/

// Initializes the application then enters a message loop which calls sample's
// RenderScene until a quit message is received.
 
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow)
{
    int failcount = 0; /* number of times RenderLoop has failed */
    MSG	msg;
    HACCEL hAccelApp;

    hPrevInstance;
    
	// Create the window and initialize all objects needed to begin rendering
    
    if(!AppInit(hInstance, lpCmdLine))
    	return FALSE;
    hAccelApp = LoadAccelerators(hInstance, "AppAccel");    

    while (!myglobs.bQuit) 
	{	 
		// Monitor the message queue until there are no pressing
		// messages
	
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
            if (msg.message == WM_QUIT) 
			{
				CleanUpAndPostQuit();
                break;
			}
            
			if (!myglobs.hWndMain || !TranslateAccelerator(myglobs.hWndMain,
							   hAccelApp, &msg)) 
			{
                TranslateMessage(&msg); 
                DispatchMessage(&msg);
            }
	 
			// If the app is not minimized, not about to quit, not paused, either the
			// active fullscreen app or in a window and D3D has been initialized,
			// we can render
	 
        } 
		else if (d3dapp->bRenderingIsOK && !d3dapp->bMinimized 
		   && !d3dapp->bPaused && !myglobs.bQuit
		   && (d3dapp->bAppActive || !d3dapp->bFullscreen)) 
		{
	    
			// If were are not in single step mode or if we are and the
			// bDrawAFrame flag is set, render one frame
			
            if (!(myglobs.bSingleStepMode && !myglobs.bDrawAFrame)) 
			{
				// Attempt to render a frame, if it fails, take a note.  If
				// rendering fails more than twice, abort execution.
		 
				if (!RenderLoop()) 
				{
					++failcount;
					if (failcount == 3) 
					{
						Msg("Rendering has failed too many times.  Aborting execution.\n");
						CleanUpAndPostQuit();
						break;
					}
				}
			}
	    
			// Reset the bDrawAFrame flag if we are in single step mode
	    
			if (myglobs.bSingleStepMode)
				myglobs.bDrawAFrame = FALSE;
        } 
		else 
		{
            WaitMessage();
		}
    } // end while

    DestroyWindow(myglobs.hWndMain);
    return msg.wParam;
}

/****************************************************************************/
/*             D3DApp Initialization and callback functions                 */
/****************************************************************************/
/*
 * AppInit
 * Creates the window and initializes all objects necessary to begin rendering
 */
static BOOL
AppInit(HINSTANCE hInstance, LPSTR lpCmdLine)
{
    WNDCLASS wc;

    /*
     * Initialize the global variables
     */
    InitGlobals();
    myglobs.hInstApp = hInstance;
    /*
     * Register the window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, "AppIcon");
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = "AppMenu";
    wc.lpszClassName = "Example";
    if (!RegisterClass(&wc))
        return FALSE;
    /*
     * Create a window with some default settings that may change
     */
    myglobs.hWndMain = CreateWindowEx(
     WS_EX_APPWINDOW,		// extended window style
	 "Example",				// pointer to registered class name
	 "RR Map Editor v1.1",		// pointer to windwos name
	 WS_SYSMENU | WS_MAXIMIZEBOX | WS_MAXIMIZE,	// windows style
	 CW_USEDEFAULT,			// Horizontal position of window
	 CW_USEDEFAULT,			// Veritcal position of window
	 640,480,				// window width , height
     NULL,				    // parent window 
	 NULL,				    // menu handle 
	 hInstance,			    // program handle 
	 NULL);				    // create parms 
	

    if (!myglobs.hWndMain)
	{
    		
    	return FALSE;
    }
	
	if(!InitInput(hInstance, myglobs.hWndMain))
		MessageBox(myglobs.hWndMain,"DirectInput initialization Failed!",NULL,MB_OK);
	
	setupinfo=LoadSetupInfo(myglobs.hWndMain);

	if(setupinfo != NULL)
	{
		current_screen_status = setupinfo->screen;
	}
	
	
    // Display the window
    if(current_screen_status != FULLSCREEN_ONLY)
	{
		ShowWindow(myglobs.hWndMain, SW_SHOWNORMAL);
		UpdateWindow(myglobs.hWndMain);
	}
    // Have the example initialize it components which remain constant
    // throughout execution
     
    if (!InitScene())
        return FALSE;
    
    // Call D3DApp to initialize all DD and D3D objects necessary to render.
    // D3DApp will call the device creation callback which will initialize the
    // viewport and the sample's execute buffers.
    
    if (!CreateD3DApp(lpCmdLine))
	return FALSE;

    return TRUE;
}


// CreateD3DApp
// Create all DirectDraw and Direct3D objects necessary to begin rendering.
// Add the list of D3D drivers to the file menu.

static BOOL CreateD3DApp(LPSTR lpCmdLine)
{
    HMENU hmenu;
    int i;
    LPSTR option;
    BOOL bOnlySystemMemory, bOnlyEmulation;
    DWORD flags;
    Defaults defaults;

    // Give D3DApp a list of textures to keep track of.
   	
	//if(LoadTextures(myglobs.hWndMain) == FALSE)
	//{
	//	MessageBox(myglobs.hWndMain,"Texture Error",NULL,MB_OK);
	//	return FALSE;
	//}

	
	LoadTextures(myglobs.hWndMain, LOAD_ALL, &world);
		
	
    // Parse the command line in seach of one of the following options:
    //     systemmemory  All surfaces should be created in system memory.
    //                   Hardware DD and D3D devices are disabled, but
    //                   debugging during the Win16 lock becomes possible.
    //     emulation     Do not use hardware DD or D3D devices.
    //

    bOnlySystemMemory = FALSE; // when TRUE same error as using new diamond drivers
    bOnlyEmulation = FALSE;		
    option = strtok(lpCmdLine, " -");
    
	while(option != NULL )   
	{
        if (!lstrcmp(option, "systemmemory")) 
		{
            bOnlySystemMemory = TRUE;
        } 
		else if (!lstrcmp(option, "emulation")) 
		{
			bOnlyEmulation = TRUE;
		} 
		else 
		{
            Msg("Invalid command line options given.\nLegal options: -systemmemory, -emulation\n");
            return FALSE;
        }
        option = strtok(NULL, " -");
    }
    
	
    // Set the flags to pass to the D3DApp creation based on command line
     
    flags = ((bOnlySystemMemory) ? D3DAPP_ONLYSYSTEMMEMORY : 0) | 
	    ((bOnlyEmulation) ? (D3DAPP_ONLYD3DEMULATION |
				 D3DAPP_ONLYDDEMULATION) : 0);
    
    // Create all the DirectDraw and D3D objects neccesary to render.  The
    // AfterDeviceCreated callback function is called by D3DApp to create the
    // viewport and the example's execute buffers.
    
    if (!D3DAppCreateFromHWND(flags, myglobs.hWndMain, AfterDeviceCreated,
			      NULL, BeforeDeviceDestroyed, NULL, &d3dapp)) 
	{
		ReportD3DAppError();
		return FALSE;
    }
    
    // Add the the list of display modes D3DApp found to the mode menu
     
    hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 5);
    for (i = 0; i < d3dapp->NumModes; i++) 
	{
        char ach[80];
        wsprintf(ach,"%dx%dx%d", d3dapp->Mode[i].w, d3dapp->Mode[i].h,
		 d3dapp->Mode[i].bpp);
		if(d3dapp->Mode[i].bThisDriverCanDo == TRUE)
			AppendMenu(hmenu, MF_STRING, MENU_FIRST_MODE+i, ach);
    }
    
    // Add the list of D3D drivers D3DApp foudn to the file menu
     
    hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 1);
    for (i = 0; i < d3dapp->NumDrivers; i++) 
	{
        InsertMenu(hmenu, 0 + i, MF_BYPOSITION | MF_STRING,
		   MENU_FIRST_DRIVER + i, d3dapp->Driver[i].Name);
    }
    
    // Allow the sample to override the default render state and other
    // settings
     
    if (!D3DAppGetRenderState(&defaults.rs)) 
	{
		ReportD3DAppError();
		return FALSE;
    }

    lstrcpy(defaults.Name, "RR Map Editor v1.1");
    defaults.bTexturesDisabled = FALSE;
    defaults.bResizingDisabled = myglobs.bResizingDisabled;
    defaults.bClearsOn = myglobs.bClearsOn;
    OverrideDefaults(&defaults);
    myglobs.bClearsOn = defaults.bClearsOn;
    myglobs.bResizingDisabled = defaults.bResizingDisabled;
    
    // Apply any changes to the render state
     
    memcpy(&myglobs.rstate, &defaults.rs, sizeof(D3DAppRenderState));
    if (!D3DAppSetRenderState(&myglobs.rstate)) 
	{
		ReportD3DAppError();
		return FALSE;
    }
    
    // If I should begin with textures off, disable them and re-create
    // the view.
     
    if (defaults.bTexturesDisabled) 
	{
		if (!D3DAppDisableTextures(defaults.bTexturesDisabled)) 
		{
			ReportD3DAppError();
			return FALSE;
		}
	
		// Release all objects (ie execute buffers) created by InitView
	 
		ReleaseView(d3dapp->lpD3DViewport);
	
		// Create the sample's execute buffers via InitView
	 
		if (!InitView(d3dapp->lpDD, d3dapp->lpD3D, d3dapp->lpD3DDevice,
		      d3dapp->lpD3DViewport, d3dapp->NumUsableTextures,
		      d3dapp->TextureHandle)) 
		{
			Msg("InitView failed.\n");
			CleanUpAndPostQuit();
			return FALSE;
		}
    }

    SetWindowText(myglobs.hWndMain, defaults.Name);

    return TRUE;
}

/*
 * AfterDeviceCreated
 * D3DApp will call this function immediately after the D3D device has been
 * created (or re-created).  D3DApp expects the D3D viewport to be created and
 * returned.  The sample's execute buffers are also created (or re-created)
 * here.
 */
static BOOL AfterDeviceCreated(int w, int h, LPDIRECT3DVIEWPORT2* lplpViewport, LPVOID lpContext)
{
    HMENU hmenu;
    int i;
    char ach[20];
    LPDIRECT3DVIEWPORT2 lpD3DViewport;
    HRESULT rval;
	DDCOLORKEY ckey;

    /*
     * Create the D3D viewport object
     */
    rval = d3dapp->lpD3D->CreateViewport(&lpD3DViewport, NULL);
    if (rval != D3D_OK) 
	{
    	Msg("Create D3D viewport failed.\n%s", D3DAppErrorToString(rval));
		CleanUpAndPostQuit();
		return FALSE;
    }
    /*
     * Add the viewport to the D3D device
     */
    rval = d3dapp->lpD3DDevice->AddViewport(lpD3DViewport);
    if (rval != D3D_OK) 
	{
    	Msg("Add D3D viewport failed.\n%s", D3DAppErrorToString(rval));
		CleanUpAndPostQuit();
		return FALSE;
    }
    /*
     * Setup the viewport for a reasonable viewing area
     */
    D3DVIEWPORT2 viewData;
    memset(&viewData, 0, sizeof(D3DVIEWPORT2));
    viewData.dwSize = sizeof(D3DVIEWPORT2);
    viewData.dwX = viewData.dwY = 0;
    viewData.dwWidth = w;
    viewData.dwHeight = h;
	viewData.dvClipX = -1.0f;
	viewData.dvClipWidth = 2.0f;
	viewData.dvClipHeight = h * 2.0f / w;
	viewData.dvClipY = viewData.dvClipHeight / 2.0f;
	viewData.dvMinZ = 0.0f;
	viewData.dvMaxZ = 1.0f;
    rval = lpD3DViewport->SetViewport2(&viewData);
    if (rval != D3D_OK) 
	{
    	Msg("SetViewport failed.\n%s", D3DAppErrorToString(rval));
		CleanUpAndPostQuit();
		return FALSE;
    }
    /*
     * Return the viewport to D3DApp so it can use it
     */
    *lplpViewport = lpD3DViewport;
    /*
     * Create the sample's execute buffers via InitView
     */
    if (!InitView(d3dapp->lpDD, d3dapp->lpD3D, d3dapp->lpD3DDevice,
		  lpD3DViewport, d3dapp->NumUsableTextures,
		  d3dapp->TextureHandle)) 
	{
		Msg("InitView failed.\n");
		CleanUpAndPostQuit();
		return FALSE;
    }
    /*
     * Add the list of texture formats found by D3DApp to the texture menu
     */
    hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 4);
    for (i = 0; i < d3dapp->NumTextureFormats; i++) 
	{
		if (d3dapp->TextureFormat[i].bPalettized) 
		{
			wsprintf(ach, "%d-bit Palettized",
		     d3dapp->TextureFormat[i].IndexBPP);
		} 
		else 
		{
			wsprintf(ach, "%d%d%d RGB", d3dapp->TextureFormat[i].RedBPP,
		     d3dapp->TextureFormat[i].GreenBPP,
		     d3dapp->TextureFormat[i].BlueBPP);
		}
        AppendMenu(hmenu, MF_STRING, MENU_FIRST_FORMAT + i, ach);
    }
    /*
     * Create and initialize the surfaces containing the frame rate and
     * window information
     */
    if (!InitFontAndTextBuffers()) 
	{
		Msg("InitFontAndTextBuffers failed.\n");
		CleanUpAndPostQuit();
		return FALSE;
	}
	ckey.dwColorSpaceLowValue=RGB_MAKE(0,0,0);
	//d3dapp->lpTextureSurf[6]->SetColorKey(DDCKEY_SRCBLT,&ckey);
    return TRUE;
}

// BeforeDeviceDestroyed
// D3DApp will call this function before the current D3D device is destroyed
// to give the app the opportunity to destroy objects it has created with the
// DD or D3D objects.

static BOOL BeforeDeviceDestroyed(LPVOID lpContext)
{
    HMENU hmenu;
    int i;
    
    // Release the font object and buffers containing stats
    
    ReleaseFontAndTextBuffers();
    
    // Release all objects (ie execute buffers) created by InitView
     
    ReleaseView(d3dapp->lpD3DViewport);
    
    // Since we created the viewport it is our responsibility to release
    // it.  Use D3DApp's pointer to it since we didn't save one.
     
    d3dapp->lpD3DViewport->Release();
    
    // Delete the list of texture formats from the texture menu because
    // they are likely to change
     
    if (myglobs.hWndMain) 
	{
		hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 3);
		if (hmenu) 
		{
			for (i = 0; i < d3dapp->NumTextureFormats; i++) 
			{
				DeleteMenu(hmenu, MENU_FIRST_FORMAT + i, MF_BYCOMMAND);
			}
		}
    }
    return TRUE;
}

/****************************************************************************/
/*                            Rendering loop                                */
/****************************************************************************/

// RenderLoop
// Render the next frame and update the window

static BOOL RenderLoop()
{
	int count;
    D3DRECT extents[D3DAPP_MAXCLEARRECTS];
	DWORD dwKeyState;
	HDC hdc;
	float k;

	
	k = (float)0.017453292;

	hdc = GetDC(myglobs.hWndMain);

	toggle_delay++;
	if(toggle_delay > 10000)
		toggle_delay = 51;


	
	// KEYBOARD 

	dwKeyState = ReadKeyboardInput();

	if((dwKeyState & KEY_SCREENSHOT) == KEY_SCREENSHOT) 
	{

		DDBitmapScreenDump(myglobs.hWndMain, d3dapp);
	}

	update_player_position_flag = FALSE;

		
	// KEYBOARD WALK MODE
		
	
	car_speed = (float)0;

	if((dwKeyState & KEY_LEFT) == KEY_LEFT) 
		temp_view_angle-=4;
				
	if((dwKeyState & KEY_RIGHT) == KEY_RIGHT) 
		temp_view_angle+=4;
			
	if((dwKeyState & KEY_MOVE_FORWARD) == KEY_MOVE_FORWARD) 			
		car_speed = (float)4;
			
	if((dwKeyState & KEY_MOVE_BACKWARD) == KEY_MOVE_BACKWARD) 
		car_speed = (float)-4;
			
	if((dwKeyState & KEY_MOVE_UP) == KEY_MOVE_UP) 			
		cam.view_point_y += 2;
			
	if((dwKeyState & KEY_MOVE_DOWN) == KEY_MOVE_DOWN) 
		cam.view_point_y -= 2;
			
	

	if(cam.view_angle<0)
		temp_view_angle += 360;

	if(cam.view_angle>=360)
		temp_view_angle = temp_view_angle -360;

	cam.direction_x = (float)sin(k * temp_view_angle);
	cam.direction_y = (float)0;
	cam.direction_z = (float)cos(k * temp_view_angle);
       
	cam.view_point_x+=(float)cam.direction_x * (float)car_speed;
	cam.view_point_z+=(float)cam.direction_z * (float)car_speed;
			
	cam.view_angle = (int)temp_view_angle;	


    // If all the DD and D3D objects have been initialized we can render
    
    if (d3dapp->bRenderingIsOK) 
	{
		// Restore any lost surfaces
	 
        if (!RestoreSurfaces()) 
		{
			// Restoring surfaces sometimes fails because the surfaces cannot
			// yet be restored.  If this is the case, the error will show up
			// somewhere else and we should return success here to prevent
			// unnecessary error's being reported.
			
			return TRUE;
		}
	
		// Calculate the frame rate
	 
        if (!CalculateFrameRate())
			return FALSE;

        // Clear the back buffer and Z buffer if enabled.  If bResized is set,
		// clear the entire window.
         
		if (myglobs.bClearsOn) 
		{
			if (!D3DAppClearBackBuffer(myglobs.bResized ?
				       D3DAPP_CLEARALL : NULL)) 
			{
				ReportD3DAppError();
				return FALSE;
			}
		}
	
		// Call the sample's RenderScene to render this frame
		
		
		// init the extent to something wrong
		extents[0].x1 = 2048;
		extents[0].x2 = 0;
		extents[0].y1 = 2048;
		extents[0].y2 = 0;
		if (!RenderScene(d3dapp->lpD3DDevice, d3dapp->lpD3DViewport,
		     &extents[0],cam)) 
		{
			Msg("RenderScene failed.\n");
			return FALSE;
		}
		count = 1;
	
	
		// Blt the frame rate and window stat text to the back buffer
	 
        if (!DisplayFrameRate(&count, &extents[1]))
		    return FALSE;
	
		// Give D3DApp the extents so it can keep track of dirty sections of
		// the back and front buffers.  Look to see if the extent from RenderScene
		// changed any.  If not, skip it's dirty rect.
	 

		if (extents[0].x1 == 2048.0) 
		{	// nothing changed
			if (!D3DAppRenderExtents(count-1, &extents[1], myglobs.bResized ?
					 D3DAPP_CLEARALL : NULL)) 
			{
				ReportD3DAppError();
				return FALSE;
			}
		}	 
		else 
		{
			// correct for not quite right extent behavior in some drivers
			if (extents[0].x1 != 0)
				extents[0].x1 -= 1;
			if (extents[0].y1 != 0)
				extents[0].y1 -= 1;
			if (!D3DAppRenderExtents(count, extents, myglobs.bResized ?
					 D3DAPP_CLEARALL : NULL)) 
			{
				ReportD3DAppError();
				return FALSE;
			}
		}
	
		// Blt or flip the back buffer to the front buffer.  Don't report an
		// error if this fails.
	 
		D3DAppShowBackBuffer(myglobs.bResized ? D3DAPP_SHOWALL : NULL);
	
		// Reset the resize flag
	 
        myglobs.bResized = FALSE;
    }
    return TRUE;
}

/*
 * AppPause
 * Pause and unpause the application
 */
static BOOL AppPause(BOOL f)
{
    
    // Flip to the GDI surface and halt rendering
    
    if (!D3DAppPause(f))
		return FALSE;
    
    // When returning from a pause, reset the frame rate count
     
    if (!f) 
	{
        ResetFrameRate();
    }
    return TRUE;
}

// RestoreSurfaces
// Restores any lost surfaces.  Returns TRUE if all surfaces are not lost and
// FALSE if one or more surfaces is lost and can not be restored at the
// moment.

static BOOL RestoreSurfaces()
{
    HRESULT d3drval;

    // Have D3DApp check all the surfaces it's in charge of
     
    if (!D3DAppCheckForLostSurfaces()) 
	{
            return FALSE;
    }
    
    // Check frame rate and info surfaces and re-write them if they
    // were lost.
    
    if (myglobs.lpFrameRateBuffer->IsLost() == DDERR_SURFACELOST) 
	{
        d3drval = myglobs.lpFrameRateBuffer->Restore();
        if (d3drval != DD_OK) 
		{
            return FALSE;
        }
		if (!WriteFrameRateBuffer(0.0f, 0))
			return FALSE;
    }
    
	if (myglobs.lpInfoBuffer->IsLost() == DDERR_SURFACELOST) 
	{
        d3drval = myglobs.lpInfoBuffer->Restore();
        if (d3drval != DD_OK) 
		{
            return FALSE;
        }
		if (!WriteInfoBuffer())
			return FALSE;
    }
    return TRUE;
}


/*************************************************************************
  Windows message handlers
 *************************************************************************/



BOOL FAR PASCAL MapEditor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int button_number;
	int toolbar_button_number;

	
	hdc=GetDC(hwnd);
	switch (msg)
	{
		case WM_INITDIALOG:
			SetTimer(hwnd,1,200,NULL);
		
			ed.wptr = &world;
			ed.scroll_bar_flag = FALSE;
			ed.current_object_id = 5;
			ed.dialogbar_mode = DIALOGBAR_MODE_ROADS;
			ed.current_object_id = OBJECT_ID_HOUSE_SEMI_LEFT;
			ed.editor_mode = 0;
			ed.Redraw_editor_map_flag = 0;
			ed.select_mode = FALSE;
			break;

		case WM_PAINT:
			DrawDialogControls(hdc, &ed);
			break;

		case WM_MOUSEMOVE:
			xPos = LOWORD(lParam);  
			yPos = HIWORD(lParam);  
			
			UpdateEditorMapWindow(hwnd, hdc, xPos, yPos, MOUSE_MOVE, &ed);
			break;

		case WM_LBUTTONDBLCLK:
			break;

		case WM_VSCROLL:
			break;
		
		case WM_MBUTTONDOWN:
			UpdateEditorMapWindow(hwnd, hdc, xPos, yPos, MIDDLE_BUTTON_DOWN, &ed);
			break;

		case WM_LBUTTONDOWN:
			SetTimer(hwnd,3,100,NULL);
			old_xPos = xPos;
			old_yPos = yPos;
			
			if(ed.select_mode == TRUE)
				UpdateEditorMapWindow(hwnd, hdc, xPos, yPos, MIDDLE_BUTTON_DOWN, &ed);
			else
				UpdateEditorMapWindow(hwnd, hdc, xPos, yPos, LEFT_BUTTON_DOWN, &ed);
			
			UpdateToolBar(hdc, xPos, yPos, LEFT_BUTTON_DOWN);
			
			button_number = UpdateScrollBars(hdc, xPos, yPos, LEFT_BUTTON_DOWN,&ed);
			if(button_number == 1)
			{
				ed.display_x_offset+=20;
				DrawEditorMap(hdc, &ed);
			}
			if(button_number == 2)
			{
				ed.display_x_offset-=20;
				DrawEditorMap(hdc, &ed);
			}
			if(button_number == 3)
			{
				ed.display_y_offset+=20;
				DrawEditorMap(hdc, &ed);
			}
			if(button_number == 4)
			{
				ed.display_y_offset-=20;
				DrawEditorMap(hdc, &ed);
			}

			UpdateDialogBar(hdc, xPos, yPos, LEFT_BUTTON_DOWN, &ed);			
		
			break;
		
		case WM_LBUTTONUP:
			KillTimer(hwnd,3);
			ed.scroll_bar_flag = FALSE;
		
			toolbar_button_number = UpdateToolBar(hdc, old_xPos, old_yPos, LEFT_BUTTON_UP);
		
			if(toolbar_button_number == 6) // Delete
				DeleteMapObject(hdc, &ed);

			if(toolbar_button_number == 7) // undo
			{
				ed.wptr->oblist_length--;

				if(ed.wptr->oblist_length < 0)
					ed.wptr->oblist_length = 0;
				UndoLastLink(&ed);
				DrawEditorMap(hdc,&ed);
			}
			if(toolbar_button_number == 8) // rotate 90 degrees clockwise
			{
				object_rot_angle+=90;
				if(object_rot_angle >359)
					object_rot_angle = object_rot_angle - 360;
				ed.object_rot_angle = object_rot_angle;
			}
			if(toolbar_button_number == 9) // zoom out
			{
				if(ed.display_scale!=0)
					ed.display_scale = ed.display_scale / 2;
				
				UpdateStatusBar(hdc, 0, 0, &ed);
				DrawEditorMap(hdc, &ed);
			}
			if(toolbar_button_number == 10)  // zoom in
			{
				ed.display_scale  = ed.display_scale * 2;
				if(ed.display_scale>4)
					ed.display_scale=(float)4;

				UpdateStatusBar(hdc, 0, 0, &ed);
				DrawEditorMap(hdc, &ed);
			}
			if(toolbar_button_number == 11)
			{
				ed.dialogbar_mode = DIALOGBAR_MODE_ROADS;
				ed.editor_mode = 0;
				DrawDialogBar(hdc, &ed);
			}

			if(toolbar_button_number == 12)
			{
				ed.dialogbar_mode = DIALOGBAR_MODE_OBJECTS;
				ed.editor_mode = 1;
				DrawDialogBar(hdc, &ed);
			}

			if(toolbar_button_number == 13)
			{
				ed.select_mode = !ed.select_mode;
			}

			UpdateStatusBar(hdc, 0, 0, &ed);

			UpdateScrollBars(hdc, xPos, yPos, LEFT_BUTTON_UP, &ed);

			button_number=UpdateDialogBar(hdc, old_xPos, old_yPos, LEFT_BUTTON_UP, &ed);
			
			if(ed.dialogbar_mode == DIALOGBAR_MODE_ROADS)
			{
				if(button_number == 1) 
					DrawRoadSection(hdc, &ed,LEFT_HAND_CURVE);

				if(button_number == 2) 
					DrawRoadSection(hdc, &ed,RIGHT_HAND_CURVE);

				if(button_number == 3)
					DrawRoadSection(hdc, &ed,STRAIGHT);
				
				if(button_number == 4)
					DrawRoadSection(hdc, &ed,SMALL_STRAIGHT);

				if(button_number == 5)
					DrawRoadSection(hdc, &ed,LEFT_CORNER);
				
				if(button_number == 6)
					DrawRoadSection(hdc, &ed,RIGHT_CORNER);

				if(button_number == 7)
					DrawRoadSection(hdc, &ed,CROSSROAD);
				
				if(button_number == 8)
					DrawRoadSection(hdc, &ed,T_JUNCTION);
			
				if(button_number == 9)
					DrawRoadSection(hdc, &ed,LEFT_T_JUNCTION);

				if(button_number == 10)
					DrawRoadSection(hdc, &ed,RIGHT_T_JUNCTION);

				if(button_number == 11)
					DrawRoadSection(hdc, &ed,ROUND_ABOUT);

				if(button_number == 12)
					DrawRoadSection(hdc, &ed,ZEBRA);

				if(button_number == 13)
					DrawRoadSection(hdc, &ed, MAINROAD_CROSSROADS );

				if(button_number == 14)
					DrawRoadSection(hdc, &ed, MAINROAD_CROSSROADS2 );

				if(button_number == 15)
					DrawRoadSection(hdc, &ed, MAINROAD_STRAIGHT);

				if(button_number == 16)
					DrawRoadSection(hdc, &ed,MAINROAD_TJUNCTION);

				if(button_number == 17)
					DrawRoadSection(hdc, &ed,MAINROAD_TJUNCTION2);

				if(button_number == 18)
					DrawRoadSection(hdc, &ed,MAINROAD_TJUNCTION3);

			}
			
			if(ed.dialogbar_mode == DIALOGBAR_MODE_OBJECTS)
			{
				if(button_number==1) 
					ed.current_object_id = OBJECT_ID_HOUSE_SEMI_LEFT;
				if(button_number==2) 
					ed.current_object_id = OBJECT_ID_HOUSE_SEMI_RIGHT;
				if(button_number==3) 
					ed.current_object_id = OBJECT_ID_LAMP_POST;
				if(button_number==4) 
					ed.current_object_id = OBJECT_ID_ROAD_SIGN;
				if(button_number==5) 
					ed.current_object_id = OBJECT_ID_ISLAND;
				if(button_number==6) 
					ed.current_object_id = OBJECT_ID_POLICE_CAR;
				if(button_number==7) 
					ed.current_object_id = OBJECT_ID_PETROL_STATION;
				if(button_number==8) 
					ed.current_object_id = OBJECT_ID_TRAFFIC_LIGHT;
				if(button_number==9) 
					ed.current_object_id = OBJECT_ID_SLOPE;
				if(button_number==10) 
					ed.current_object_id = OBJECT_ID_SLOPE_CORNER;
				if(button_number==11) 
					ed.current_object_id = OBJECT_ID_CENTRAL_RESERVATION;
				if(button_number==12) 
					ed.current_object_id = OBJECT_ID_MBSHOP1;
				
			}
			break;

		case WM_NCLBUTTONUP:
			KillTimer(hwnd,3);
			ed.scroll_bar_flag = FALSE;
			UpdateToolBar(hdc, old_xPos, old_yPos, LEFT_BUTTON_UP);
			UpdateScrollBars(hdc, xPos, yPos, LEFT_BUTTON_UP, &ed);
			UpdateDialogBar(hdc, old_xPos, old_yPos, LEFT_BUTTON_UP, &ed);
			break;

		case WM_RBUTTONDOWN:
			object_rot_angle+=90;
			if(object_rot_angle >359)
				object_rot_angle = object_rot_angle - 360;
			ed.object_rot_angle = object_rot_angle;
			DrawEditorMap(hdc, &ed);
			if((ed.select_mode == FALSE) && (ed.editor_mode == 1))
				DrawEditorMapObject(hdc,&ed);
			break;

		case WM_TIMER:
			if(wParam == 1)
			{
				KillTimer(hwnd,1);
				LoadTextures(myglobs.hWndMain, MAPPING_ONLY, &world);
				LoadObjectData(hwnd,"objects.dat",&world);
				DrawDialogControls(hdc, &ed);
				DrawEditorMap(hdc, &ed);
			}
			if(wParam == 2)
			{
				KillTimer(hwnd,2);
				if(ed.select_mode == FALSE)
				{
					DrawEditorMapObject(hdc,&ed);
					ed.Redraw_editor_map_flag = 1;
				}
			}
			if(wParam == 3)
			{
				button_number = UpdateScrollBars(hdc, xPos, yPos, LEFT_BUTTON_DOWN,&ed);
				if(button_number == 1)
				{
					ed.display_x_offset+=20;
					DrawEditorMap(hdc, &ed);
				}
				if(button_number == 2)
				{
					ed.display_x_offset-=20;
					DrawEditorMap(hdc, &ed);
				}
				if(button_number == 3)
				{
					ed.display_y_offset+=20;
					DrawEditorMap(hdc, &ed);
				}
				if(button_number == 4)
				{
					ed.display_y_offset-=20;
					DrawEditorMap(hdc, &ed);
				}
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case ID_FILE_EXIT:
					EndDialog(hwnd, TRUE);
					break;

				case IDCLOSE:
				case IDCANCEL:
				case IDOK:	
					PrintMsgY = 50;
					InitWorldMap(hwnd, &world);
					//MessageBeep(0);
					EndDialog(hwnd, TRUE);
					break;
					
				case IDC_SCROLLBAR1:
					break;

				case IDC_SCROLLBAR2:
					break;

				case ID_MODE_AUTOROAD:
					ed.dialogbar_mode = DIALOGBAR_MODE_ROADS;
					ed.editor_mode = 0;
					DrawDialogBar(hdc, &ed);
					break;

				case ID_MODE_MANUAL:
					ed.dialogbar_mode = DIALOGBAR_MODE_OBJECTS;
					ed.editor_mode = 1;
					DrawDialogBar(hdc, &ed);
					break;

				case ID_INSERT_ROUNDABOUT:
					break;
				
				case ID_INSERT_HOUSE:
					break;

				case ID_INSERT_OBJECT:
					break;

				case IDC_SCROLL_UP:
					ed.display_y_offset -=10;
					DrawEditorMap(hdc, &ed);
					break;

				case IDC_SCROLL_DOWN:
					ed.display_y_offset +=10;
					DrawEditorMap(hdc, &ed);
					break;

				case IDC_SCROLL_LEFT:
					ed.display_x_offset -=10;
					DrawEditorMap(hdc, &ed);
					break;

				case IDC_SCROLL_RIGHT:
					ed.display_x_offset +=10;
					DrawEditorMap(hdc, &ed);
					break;
				
				case ID_VIEW_ZOOM_IN:
					ed.display_scale  = ed.display_scale * 2;
					if(ed.display_scale>4)
						ed.display_scale=(float)4;

					DrawEditorMap(hdc, &ed);
					break;

				case ID_VIEW_ZOOM_OUT:
					if(ed.display_scale!=0)
						ed.display_scale = ed.display_scale / 2;
					
					DrawEditorMap(hdc, &ed);
					break;
				
				case IDC_LINK_NEXT:
					ChangeCurrentLink(1); // forward
					DrawEditorMap(hdc,&ed);
					DisplayLinksList(hdc);
					break;
				
				case IDC_LINK_PREV:
					ChangeCurrentLink(-1); // backward
					DrawEditorMap(hdc,&ed);
					DisplayLinksList(hdc);
					break;

				case ID_EDIT_DELETE:
					DeleteMapObject(hdc, &ed);
					break;

				case ID_FILE_NEW:
					InitLinkList(&ed);
					ed.wptr->oblist_length = 0;

					for(cell_z = 0; cell_z < MAX_NUM_MAP_CELLS; cell_z++)
					{
						for(cell_x = 0; cell_x < MAX_NUM_MAP_CELLS; cell_x++)
							ed.wptr->cell[cell_x][cell_z] = 0;
					}
					DrawEditorMap(hdc,&ed);
					break;
				
				case ID_EDIT_UNDO:
					ed.wptr->oblist_length--;

					if(ed.wptr->oblist_length < 0)
						ed.wptr->oblist_length = 0;
					UndoLastLink(&ed);
					DrawEditorMap(hdc,&ed);
					//MessageBeep(0);
					break;

				case ID_VIEW_UPDATE:
					PrintMsgY = 50;
					InitWorldMap(hwnd, &world);
					break;

				case ID_FILE_OPEN:	
					InitLinkList(&ed);
					LoadMapInEditor(hwnd,"Level1.map",&world);
					DrawEditorMap(hdc,&ed);
					break;

				case ID_FILE_SAVE:
					SaveMap(hwnd,"Level1.map",&world);
					break;
			}
			break;
		
	}
	ReleaseDC(hwnd,hdc);
	return FALSE;
}



// AppAbout
// About box message handler

BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	MEMORYSTATUS memStatus;
//	int mem;	
//	char  buffer[255];
	//LPTSTR buffer2;


	hdc=GetDC(hwnd);

	switch (msg)
	{
		case WM_INITDIALOG:
			SetTimer(hwnd,1,100,NULL);
			break;

		case WM_LBUTTONDOWN:
			break;
			
		case WM_RBUTTONDOWN:
			break;

		case WM_TIMER:
			KillTimer(hwnd,1);
			
			DisplayRRStats(hwnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDOK:
				case IDCANCEL:      
					EndDialog(hwnd, TRUE);
					break;

				case IDC_BUTTON1:
					DisplayCredits(hwnd);
					break;
				
				case IDC_BUTTON2:
					DisplayRRStats(hwnd);
					break;

				case IDC_BUTTON3:
					DisplayControls(hwnd);
					break;

				case IDC_BUTTON4:
					DisplayLegalInfo(hwnd);	
					break;
			break;
			}
			
	break;
		
	}
  ReleaseDC(hwnd,hdc);  
  return FALSE;
}



// WindowProc
// Main window message handler.
 
long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
						   WPARAM wParam, LPARAM lParam )
{	
	UINT cmd;
    BOOL bStop;
	BOOL bIsActive;
    LRESULT lresult;
	HINSTANCE hInst;
//	HINSTANCE hInst2;
	int i;
//	HWND hwnd2;


	hInst=myglobs.hInstApp;

    // Give D3DApp an opportunity to process any messages it MUST see in order
    // to perform it's function.
    
    if (!D3DAppWindowProc(&bStop, &lresult, hWnd, message, wParam, lParam)) 
	{
		ReportD3DAppError();
		CleanUpAndPostQuit();
		return 0;
    }

    // Prevent resizing through this message
     
    if (message == WM_GETMINMAXINFO && myglobs.bResizingDisabled && !d3dapp->bFullscreen && !d3dapp->bMinimized) 
	{
		RECT rc;
		GetWindowRect(hWnd, &rc);
		((LPMINMAXINFO)lParam)->ptMaxTrackSize.x = START_WIN_SIZE;
		((LPMINMAXINFO)lParam)->ptMaxTrackSize.y = START_WIN_SIZE;
		((LPMINMAXINFO)lParam)->ptMinTrackSize.x = START_WIN_SIZE;
		((LPMINMAXINFO)lParam)->ptMinTrackSize.y = START_WIN_SIZE;
        return 0;
    }
 
    // If bStop is set by D3DApp, the app should not process the message but
    // return lresult.
     
    if (bStop)	return lresult;
    
	switch( message ) 
	{	
		case WM_ACTIVATEAPP:
			bIsActive = (BOOL) wParam;
		
			if( bIsActive )
    		{     
				// we are active, need to reacquire the keyboard
				ReacquireInput();
			
			}
			break;

		case WM_CREATE:
			car_speed=(float)0;

			cam.direction_x=(float)0;
			cam.direction_y=(float)0;
			cam.direction_z=(float)0;
			
			cam.view_angle=90;
			cam.view_point_x=(float)720;
			cam.view_point_y=(float)14;
			cam.view_point_z=(float)450;
			
			cam.ramp_size=1;

			ed.position.x=(float)200;
			ed.position.y=(float)0;
			ed.position.z=(float)200;

			ed.pave_width=(float)20;
			ed.road_width=(float)120;
			ed.num_sections=3;
			ed.angle=(float)0;
			ed.delta_angle=(float)5;
			ed.display_scale=(float).25;
			ed.display_x_offset=0;	
			ed.display_y_offset=0;
			ed.draw_road_section_flags=ALL;
			InitLinkList(&ed);

			LoadToolbarBitmaps(hInst);
			temp_view_angle = (float)cam.view_angle;
			
			SetTimer(hWnd,1,1000,NULL);
			break;
		
		case WM_TIMER:
			KillTimer(hWnd,1);		
			
			LoadObjectData(hWnd,"objects.dat",&world);
			LoadWorldMap  (hWnd,"level1.map",&world);
			InitPreCompiledWorldMap(hWnd, &world);

			if(current_screen_status == FULLSCREEN_ONLY)
				D3DAppFullscreen(d3dapp->CurrMode);

			if (!D3DAppSetRenderState(&myglobs.rstate)) 
			{
				ReportD3DAppError();
				break;
			}
			
        case WM_LBUTTONDOWN:
			break;

		case WM_RBUTTONDOWN:
		MessageBeep(0);
			break;

        case WM_KEYDOWN:
			break;
        
		// Pause and unpause the app when entering/leaving the menu
	 
        case WM_ENTERMENULOOP:
            AppPause(TRUE);
            break;
        case WM_EXITMENULOOP:
            AppPause(FALSE);
            break;
    	case WM_DESTROY:
            myglobs.hWndMain = NULL;
            CleanUpAndPostQuit();
            break;
        
		case WM_INITMENUPOPUP:	    
			// Check and enable the appropriate menu items
			CheckMenuItem((HMENU)wParam,ID_LIGHTING_RAMPEFFECT, (cam.ramp_size==16) ? MF_CHECKED : MF_UNCHECKED);
			
			if (d3dapp->ThisDriver.bDoesZBuffer) 
			{
				EnableMenuItem((HMENU)wParam, MENU_ZBUFFER, MF_ENABLED);
				CheckMenuItem((HMENU)wParam, MENU_ZBUFFER, myglobs.rstate.bZBufferOn ? MF_CHECKED : MF_UNCHECKED);
			} 
			else 
			{
				EnableMenuItem((HMENU)wParam, MENU_ZBUFFER, MF_GRAYED);
				CheckMenuItem((HMENU)wParam, MENU_ZBUFFER, MF_UNCHECKED);
			}
            
			CheckMenuItem((HMENU)wParam, MENU_STEP, (myglobs.bSingleStepMode) ? MF_CHECKED : MF_UNCHECKED);
			EnableMenuItem((HMENU)wParam, MENU_GO, (myglobs.bSingleStepMode) ? MF_ENABLED : MF_GRAYED);
			CheckMenuItem((HMENU)wParam, MENU_FLAT, (myglobs.rstate.ShadeMode == D3DSHADE_FLAT) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_GOURAUD, (myglobs.rstate.ShadeMode == D3DSHADE_GOURAUD) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_PHONG, (myglobs.rstate.ShadeMode == D3DSHADE_PHONG) ? MF_CHECKED : MF_UNCHECKED);
			EnableMenuItem((HMENU)wParam, MENU_PHONG, MF_GRAYED);
			CheckMenuItem((HMENU)wParam, MENU_CLEARS, myglobs.bClearsOn ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_POINT, (myglobs.rstate.FillMode == D3DFILL_POINT) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_WIREFRAME, (myglobs.rstate.FillMode == D3DFILL_WIREFRAME) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_SOLID, (myglobs.rstate.FillMode == D3DFILL_SOLID) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_DITHERING, myglobs.rstate.bDithering ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_SPECULAR, myglobs.rstate.bSpecular ? MF_CHECKED : MF_UNCHECKED);
			EnableMenuItem((HMENU)wParam, MENU_SPECULAR, MF_ENABLED);
			CheckMenuItem((HMENU)wParam, MENU_FOG, myglobs.rstate.bFogEnabled ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_ANTIALIAS, myglobs.rstate.bAntialiasing ? MF_CHECKED : MF_UNCHECKED);
	    
			if (d3dapp->ThisDriver.bDoesTextures) 
			{
				CheckMenuItem((HMENU)wParam, MENU_TEXTURE_TOGGLE, (!d3dapp->bTexturesDisabled) ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_TEXTURE_TOGGLE, MF_ENABLED);
				EnableMenuItem((HMENU)wParam, MENU_TEXTURE_SWAP, (d3dapp->bTexturesDisabled) ? MF_GRAYED : MF_ENABLED);
				CheckMenuItem((HMENU)wParam, MENU_PERSPCORRECT, myglobs.rstate.bPerspCorrect ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_PERSPCORRECT, (d3dapp->bTexturesDisabled) ? MF_GRAYED : MF_ENABLED);
				CheckMenuItem((HMENU)wParam, MENU_POINT_FILTER, (myglobs.rstate.TextureFilter == D3DFILTER_NEAREST) ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_POINT_FILTER, (d3dapp->bTexturesDisabled) ? MF_GRAYED : MF_ENABLED);
				CheckMenuItem((HMENU)wParam, MENU_LINEAR_FILTER, (myglobs.rstate.TextureFilter == D3DFILTER_LINEAR) ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_LINEAR_FILTER, (d3dapp->bTexturesDisabled) ? MF_GRAYED : MF_ENABLED);
		
				for (i = 0; i < d3dapp->NumTextureFormats; i++) 
				{
					CheckMenuItem((HMENU)wParam, MENU_FIRST_FORMAT + i, (i == d3dapp->CurrTextureFormat) ? MF_CHECKED : MF_UNCHECKED);
					EnableMenuItem((HMENU)wParam, MENU_FIRST_FORMAT + i, (d3dapp->bTexturesDisabled) ? MF_GRAYED : MF_ENABLED);
				}
			} 
			else 
			{
				EnableMenuItem((HMENU)wParam, MENU_TEXTURE_SWAP, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_TEXTURE_TOGGLE, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_POINT_FILTER, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_LINEAR_FILTER, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_PERSPCORRECT, MF_GRAYED);
			}
	    
			if (d3dapp->bIsPrimary) 
			{
				CheckMenuItem((HMENU)wParam, MENU_FULLSCREEN, d3dapp->bFullscreen ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_FULLSCREEN, d3dapp->bFullscreen && !d3dapp->ThisDriver.bCanDoWindow ? MF_GRAYED : MF_ENABLED);
				EnableMenuItem((HMENU)wParam, MENU_NEXT_MODE, (!d3dapp->bFullscreen) ? MF_GRAYED : MF_ENABLED);
				EnableMenuItem((HMENU)wParam, MENU_PREVIOUS_MODE, (!d3dapp->bFullscreen) ? MF_GRAYED : MF_ENABLED);
			} 
			else 
			{
				EnableMenuItem((HMENU)wParam, MENU_FULLSCREEN, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_NEXT_MODE, MF_GRAYED);
				EnableMenuItem((HMENU)wParam, MENU_PREVIOUS_MODE, MF_GRAYED);
			}
	    
			for (i = 0; i < d3dapp->NumModes; i++) 
			{	
				CheckMenuItem((HMENU)wParam, MENU_FIRST_MODE + i, (i == d3dapp->CurrMode) ? MF_CHECKED : MF_UNCHECKED);
				EnableMenuItem((HMENU)wParam, MENU_FIRST_MODE + i, (d3dapp->Mode[i].bThisDriverCanDo) ? MF_ENABLED : MF_GRAYED);
			}
        
			for (i = 0; i < d3dapp->NumDrivers; i++) 
			{
				CheckMenuItem((HMENU)wParam, MENU_FIRST_DRIVER + i, (i == d3dapp->CurrDriver) ? MF_CHECKED : MF_UNCHECKED);
			}
			break;

		case WM_GETMINMAXINFO:
			// Some samples don't like being resized, such as those which use
			// screen coordinates (TLVERTEXs).
	     
			if (myglobs.bResizingDisabled) 
			{
				((LPMINMAXINFO)lParam)->ptMaxTrackSize.x = START_WIN_SIZE;
				((LPMINMAXINFO)lParam)->ptMaxTrackSize.y = START_WIN_SIZE;
				((LPMINMAXINFO)lParam)->ptMinTrackSize.x = START_WIN_SIZE;
				((LPMINMAXINFO)lParam)->ptMinTrackSize.y = START_WIN_SIZE;
				return 0;
			}
			break;
        
		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				cmd = GET_WM_COMMAND_ID(wParam, lParam);

				if (cmd >= IDC_DEVICES && cmd < IDC_DEVICES + 100) 
				{
					PickInputDevice(cmd - IDC_DEVICES);
					//MessageBeep(0);
				} 

			case ID_LIGHTING_RAMPEFFECT:
				if(cam.ramp_size==16)
					cam.ramp_size=1;
				else
					cam.ramp_size=16;
				break;
			
			case ID_TOOLS_MAPEDITOR:
				AppPause(TRUE);
				DialogBox(myglobs.hInstApp, "Editor", myglobs.hWndMain, (DLGPROC)MapEditor);
				AppPause(FALSE);
				break;
			
            case MENU_ABOUT:
                AppPause(TRUE);
                DialogBox(myglobs.hInstApp, "AppAbout", myglobs.hWndMain, (DLGPROC)AppAbout);
                AppPause(FALSE);
                break;
                
			case MENU_EXIT:
				CleanUpAndPostQuit();
        		break;
                
			case MENU_STEP:			 
				// Begin single step more or draw a frame if in single
				// step mode
		     
                if (!myglobs.bSingleStepMode) 
				{
					myglobs.bSingleStepMode = TRUE;
                    myglobs.bDrawAFrame = TRUE;
                } 
				else if (!myglobs.bDrawAFrame) 
				{
					myglobs.bDrawAFrame = TRUE;
                }
                break;
                
			case MENU_GO:		    
				// Exit single step mode
		     
                if (myglobs.bSingleStepMode) 
				{
                    myglobs.bSingleStepMode = FALSE;
                    ResetFrameRate();
                }
                break;

            case MENU_STATS:
				// Toggle output of frame rate and window info
		     
                if ((myglobs.bShowFrameRate) && (myglobs.bShowInfo)) 
				{
                    myglobs.bShowFrameRate = FALSE;
                    myglobs.bShowInfo = FALSE;
                    break;
                }
                if ((!myglobs.bShowFrameRate) && (!myglobs.bShowInfo)) 
				{
                    myglobs.bShowFrameRate = TRUE;
                    break;
                }
                myglobs.bShowInfo = TRUE;
                break;
        		
			case MENU_FULLSCREEN:
				
				if (d3dapp->bFullscreen) 
				{
					// Return to a windowed mode.  Let D3DApp decide which
					// D3D driver to use in case this one cannot render to
					// the Windows display depth
			 
					if (!D3DAppWindow(D3DAPP_YOUDECIDE, D3DAPP_YOUDECIDE)) 
					{
						ReportD3DAppError();
						CleanUpAndPostQuit();
						break;
					}
				} 
				else 
				{
					// Enter the current fullscreen mode.  D3DApp may
					// resort to another mode if this driver cannot do
					// the currently selected mode.
			 
					if (!D3DAppFullscreen(d3dapp->CurrMode)) 
					{
						ReportD3DAppError();
						CleanUpAndPostQuit();
						break;
					}
				}
				break;
		
				// Texture filter method selection
		 
            case MENU_POINT_FILTER:
				if (myglobs.rstate.TextureFilter == D3DFILTER_NEAREST)
					break;
                myglobs.rstate.TextureFilter = D3DFILTER_NEAREST;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_LINEAR_FILTER:
				if (myglobs.rstate.TextureFilter == D3DFILTER_LINEAR)
					break;
                myglobs.rstate.TextureFilter = D3DFILTER_LINEAR;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;
		 
			// Shading selection
		 
            case MENU_FLAT:
				if (myglobs.rstate.ShadeMode == D3DSHADE_FLAT)
					break;
                myglobs.rstate.ShadeMode = D3DSHADE_FLAT;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

			case MENU_GOURAUD:
                if (myglobs.rstate.ShadeMode == D3DSHADE_GOURAUD)
					break;
                myglobs.rstate.ShadeMode = D3DSHADE_GOURAUD;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;

            case MENU_PHONG:
				if (myglobs.rstate.ShadeMode == D3DSHADE_PHONG)
					break;
                myglobs.rstate.ShadeMode = D3DSHADE_PHONG;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;
		
			// Fill mode selection
		 
            case MENU_POINT:
				if (myglobs.rstate.FillMode == D3DFILL_POINT)
					break;
                myglobs.rstate.FillMode = D3DFILL_POINT;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;

            case MENU_WIREFRAME:
                if (myglobs.rstate.FillMode == D3DFILL_WIREFRAME)
					break;
                myglobs.rstate.FillMode = D3DFILL_WIREFRAME;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;

            case MENU_SOLID:
                if (myglobs.rstate.FillMode == D3DFILL_SOLID)
					break;
                myglobs.rstate.FillMode = D3DFILL_SOLID;
                
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;

            case MENU_PERSPCORRECT:
		    
				// Toggle perspective correction
		    
		        myglobs.rstate.bPerspCorrect = !myglobs.rstate.bPerspCorrect;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
                break;

            case MENU_CLEARS:
		    
				// Toggle the clearing the the back buffer and Z-buffer
				// and set the resized flag to clear the entire window
		     
                myglobs.bClearsOn = !myglobs.bClearsOn;
                
				if (myglobs.bClearsOn) 
					myglobs.bResized = TRUE;
                
				break;

            case MENU_ZBUFFER:
		    
				// Toggle the use of a Z-buffer
		    
                myglobs.rstate.bZBufferOn = !myglobs.rstate.bZBufferOn;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_DITHERING:
		    
				// Toggle dithering
		     
                myglobs.rstate.bDithering = !myglobs.rstate.bDithering;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_SPECULAR:
		    
				// Toggle specular highlights
		     
                myglobs.rstate.bSpecular = !myglobs.rstate.bSpecular;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_FOG:
		    
				// Toggle fog
		     
                myglobs.rstate.bFogEnabled = !myglobs.rstate.bFogEnabled;
                if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_ANTIALIAS:
		    
				// Toggle anti-aliasing
		    
				myglobs.rstate.bAntialiasing = !myglobs.rstate.bAntialiasing;
            
				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
				break;

            case MENU_TEXTURE_TOGGLE:		    
				// Release the sample's execute buffers, toggle the
				// texture disabled state and recreate them.
		     
				ReleaseView(d3dapp->lpD3DViewport);
				D3DAppDisableTextures(!d3dapp->bTexturesDisabled);
				
				if (!InitView(d3dapp->lpDD, d3dapp->lpD3D,
				      d3dapp->lpD3DDevice,
				      d3dapp->lpD3DViewport,
				      d3dapp->NumUsableTextures,
				      d3dapp->TextureHandle)) 
				{
					Msg("InitView failed.\n");
					CleanUpAndPostQuit();
					break;
				}
			
				myglobs.bResized = TRUE;
				break;

			case MENU_TEXTURE_SWAP:
		    
				// Swap textures using the load command
		     
				if (!D3DAppSwapTextures()) 
				{
					ReportD3DAppError();
					break;
				}
		    
				// Just in case we have a texture background
		     
				myglobs.bResized = TRUE;
				break;

			case MENU_NEXT_MODE:
		    
				// Enter the next usable fullscreen mode
		     
				i = d3dapp->CurrMode;
				do 
				{
					++i;
					if (i >= d3dapp->NumModes)
						i = 0;
					if (!d3dapp->Mode[i].bThisDriverCanDo)
						continue;
					else 
					{
						if (!D3DAppFullscreen(i)) 
						{
							ReportD3DAppError();
							CleanUpAndPostQuit();
						}
						break;
					}
				} 
				while(i != d3dapp->CurrMode);
				break;

			case MENU_PREVIOUS_MODE:		    
				// Enter the previous usable fullscreen mode
		    
				i = d3dapp->CurrMode;
				do 
				{
					--i;
					if (i < 0)
						i = d3dapp->NumModes - 1;
					
					if (!d3dapp->Mode[i].bThisDriverCanDo)
						continue;
					else 
					{
						if (!D3DAppFullscreen(i)) 
						{
							ReportD3DAppError();
							CleanUpAndPostQuit();
						}
						break;
					}
				} 
				while(i != d3dapp->CurrMode);
                break;

		} // end switch WM_COMMAND

        if (   LOWORD(wParam) >= MENU_FIRST_FORMAT
			&& LOWORD(wParam) < MENU_FIRST_FORMAT +
			D3DAPP_MAXTEXTUREFORMATS
			&& d3dapp->CurrTextureFormat !=
			LOWORD(wParam) - MENU_FIRST_FORMAT) 
		{
			// Release the sample's execute buffers, change the texture
			// format and recreate the view.
		
			ReleaseView(d3dapp->lpD3DViewport);
			if (!D3DAppChangeTextureFormat(LOWORD(wParam) - 
				MENU_FIRST_FORMAT)) 
			{
				ReportD3DAppError();
				CleanUpAndPostQuit();
			}
			
			if (!InitView(d3dapp->lpDD, d3dapp->lpD3D,
				d3dapp->lpD3DDevice, d3dapp->lpD3DViewport,
				d3dapp->NumUsableTextures,
				d3dapp->TextureHandle)) 
			{
				Msg("InitView failed.\n");
				CleanUpAndPostQuit();
				break;
			}
		
			ResetFrameRate();
        }

        if (   LOWORD(wParam) >= MENU_FIRST_DRIVER
			&& LOWORD(wParam) < MENU_FIRST_DRIVER + D3DAPP_MAXD3DDRIVERS
			&& d3dapp->CurrDriver != LOWORD(wParam) - MENU_FIRST_DRIVER) 
		{
			// Change the D3D driver
		
			if (!D3DAppChangeDriver(LOWORD(wParam) - MENU_FIRST_DRIVER,NULL)) 
			{
				ReportD3DAppError();
				CleanUpAndPostQuit();
			}	
        }
            
		if (   LOWORD(wParam) >= MENU_FIRST_MODE
			&& LOWORD(wParam) < MENU_FIRST_MODE+100) 
		{
			// Switch to the selected fullscreen mode
		 
			if (!D3DAppFullscreen(LOWORD(wParam) - MENU_FIRST_MODE)) 
			{
				ReportD3DAppError();
				CleanUpAndPostQuit();
			}
        }
	    
		// Whenever we receive a command in single step mode, 
		// draw a frame
	     
		if (myglobs.bSingleStepMode) 
			myglobs.bDrawAFrame = TRUE;
        
		return 0L;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}


/****************************************************************************/
/*                 Setting up callbacks for examples                        */
/****************************************************************************/
 /*
  * SetMouseCallback
  * Called in an example to set a callback function for all WM messages
  * dealing with the mouse.  The handler should return whether or not
  * it handled the message.
  */
BOOL
SetMouseCallback(BOOL(*Handler)(UINT, WPARAM, LPARAM))
{
    MouseHandler = Handler;
    return TRUE;
}
 
/*
 * SetKeyboardCallback
 * Called in an example to set a callback function for all WM messages
 * dealing with the keyboard. The handler should return whether or not it
 * handled the message.
 */
BOOL
SetKeyboardCallback(BOOL(*Handler)(UINT, WPARAM, LPARAM)) {
    
    KeyboardHandler = Handler;
    return TRUE;
}

/****************************************************************************/
/*          Initialization, error reporting and release functions.          */
/****************************************************************************/
/*
 * InitGlobals
 * Called once at program initialization to initialize global variables.
 */
static void
InitGlobals(void)
{
    d3dapp = NULL;
    memset(&myglobs.rstate, 0, sizeof(myglobs.rstate));
    memset(&myglobs, 0, sizeof(myglobs));
    myglobs.bClearsOn = TRUE;
    myglobs.bShowFrameRate = FALSE; //TRUE;
    myglobs.bShowInfo = FALSE; //TRUE;
    MouseHandler = NULL;
    KeyboardHandler = NULL;
}

/*
 * CleanUpAndPostQuit
 * Release all D3D objects, post a quit message and set the bQuit flag
 */
void
CleanUpAndPostQuit(void)
{
    if (myglobs.bQuit)
	return;
    if (!D3DAppDestroy())
	ReportD3DAppError();
    ReleaseScene();
    myglobs.bQuit = TRUE;
    PostQuitMessage( 0 );
}

/*
 * ReportD3DAppError
 * Reports an error during a d3d app call.
 */
void
ReportD3DAppError(void)
{
    Msg("%s", D3DAppLastErrorString());
}

/* Msg
 * Message output for error notification.
 */
void __cdecl
Msg( LPSTR fmt, ... )
{
    char buff[256];
    va_list va;

    va_start( va, fmt );
    wvsprintf( buff, fmt, va );
    va_end( va );
    lstrcat(buff, "\r\n");
    AppPause(TRUE);
    if (d3dapp && d3dapp->bFullscreen)
	SetWindowPos(myglobs.hWndMain, HWND_NOTOPMOST, 0, 0, 0, 0,
		     SWP_NOSIZE | SWP_NOMOVE);
    MessageBox( myglobs.hWndMain, buff, "D3D Example Message", MB_OK );
    if (d3dapp && d3dapp->bFullscreen)
	SetWindowPos(myglobs.hWndMain, HWND_TOPMOST, 0, 0, 0, 0,
		     SWP_NOSIZE | SWP_NOMOVE);
    AppPause(FALSE);
}

world_ptr GetWorldData()
{
	return &world;
}

