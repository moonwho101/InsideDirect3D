
// Road Rage 1.1

// FILE : d3dmain.cpp
// This file was originally part of the Microsoft DirectX5 SDK
// Edited by William Chin
// Edited with MSDEV C++ v6.0 using the DirectX 6.0 SDK
// Edited on:  3/7/99


// Make sure all header files with guid definitions 
// are listed here, to avoid guid link errors. 
// Note this version of RR does not use dxguid.lib.

#define INITGUID

#define TEXTURESIZE256X256 256
#define TEXTURESIZE128X128 128
#define TEXTURESIZE64X64   64
			

#include "d3dmain.hpp"
#include "d3dappi.hpp"
#include "world.hpp"
#include "bitmap.hpp"
#include "input.hpp"
#include "sound.hpp"
#include "texture.hpp"
#include "error.hpp"
#include <winbase.h>
#include <dinput.h>
#include <dsound.h>
#include <dplay.h>
#include <dplobby.h>
#include "dpchat.h"
#include <mmsystem.h>
#include "comm.h"
#include "ddutil.hpp"
#include "info.hpp"


// GLOBAL VARIABLES
 
D3DAppInfo* d3dapp;	    /* Pointer to read only collection of DD and D3D
						   objects maintained by D3DApp */
d3dmainglobals myglobs;	/* collection of global variables */

char D3Ddevicename[256];
char D3Ddevicedesc[256];
char FindD3Ddevicename[256];
char D3Ddevicelist[10][256];
int numD3Ddevices;
int menunumD3Ddevices;
BOOL CALLBACK UpdatePosCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
BOOL CALLBACK UpdateControlsCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

void IsPlayerHit();
void UpdateControls();

WORLD world;
CAMERA cam;
setupinfo_ptr setupinfo;

LPDIRECTDRAWSURFACE7 lpds_crosshair;
LPDIRECTDRAWSURFACE7 lpds_background;
RECT rc_crosshair;

float gv_gradient;		
float gv_dz, gv_dx, gv_da; 
float gv_target_angle;
float gv_distance;	
float gv_hit_width;	
float gv_temp_angle;
float gv_quadrant;
static float k = (float)0.017453292;
char buffer[256];
char buffer2[256];
char buffer3[256];
char buffer4[256];

BOOL d3dcard_driver_flag;
BOOL App_starting_for_first_time = TRUE;
BOOL RRAppActive = FALSE;
BOOL Joystick_Enabled_Flag;
BOOL Car_Sound_On_Flag = TRUE;
BOOL update_player_position_flag;
BOOL firing_gun_flag = FALSE;
BOOL delay_key_press_flag = FALSE;
BOOL multiplay_flag = FALSE;
BOOL am_i_the_server = FALSE;
static HRESULT hr;
DWORD last_error;
DPLAYINFO	DPInfo;


int keycode;
int car_gear=1;
int editor_insert_mode=0;
int MouseX,MouseY;
int xPos,yPos;
int current_mode;
int object_rot_angle =0;	
int old_xPos, old_yPos;
int toggle_delay = 0;
int current_screen_status = FULLSCREEN_ONLY; // default, in case rrsetup.ini 
		
int mod_id;	 
int curr_seq;  
int curr_frame;
int stop_frame;  
										
float temp_view_angle;
char szDevice_debug[256];
RECT rc_background;
extern DWORD MyRRnetID;

/*
 *  INTERNAL FUNCTION PROTOTYPES
 */
static BOOL AppInit(HINSTANCE hInstance, LPSTR lpCmdLine);
static BOOL CreateD3DApp(LPSTR lpCmdLine);
static BOOL BeforeDeviceDestroyed(LPVOID lpContext);
static BOOL AfterDeviceCreated(int w, int h,
			       LPVOID lpContext);

static void InitGlobals(void);
static BOOL AppPause(BOOL f);
void ReportD3DAppError(void);
static BOOL RenderLoop(void);
static BOOL RestoreSurfaces();
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam,
			   LPARAM lParam );

BOOL CALLBACK BuildDeviceMenuCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam, HMONITOR hm);
BOOL CALLBACK FindDeviceCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam);
void BuildDeviceMenu();
BOOL LoadRR_Resources();
void LoadCrossHair();
void LoadBackground();
void DisplayScores();
			

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

	// Initialize COM library
	hr = CoInitialize(NULL);
	if ( FAILED(hr) )
	{
		ShutdownConnection(&DPInfo);
		CoUninitialize();
	}
	
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
				PrintMessage(myglobs.hWndMain, "WinMain", NULL, LOGFILE_ONLY);
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
						PrintMessage(myglobs.hWndMain, "WinMain", NULL, LOGFILE_ONLY);
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
static BOOL AppInit(HINSTANCE hInstance, LPSTR lpCmdLine)
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
	 "Road Rage 1.1",		// pointer to windwos name
	 WS_SYSMENU, // | WS_MAXIMIZEBOX | WS_MAXIMIZE,	// windows style
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
	{
		PrintMessage(myglobs.hWndMain,"ERROR DirectInput initialization Failed! \n", NULL, LOGFILE_ONLY);
		MessageBox(myglobs.hWndMain,"DirectInput initialization Failed!",NULL,MB_OK);	
	}
	else
		PrintMessage(myglobs.hWndMain,"DirectInput initialised OK \n", NULL, LOGFILE_ONLY);

	setupinfo = LoadSetupInfo(myglobs.hWndMain);

	if(setupinfo != NULL)
	{
		current_screen_status = setupinfo->screen;
		Car_Sound_On_Flag = setupinfo->sound;
	}
	
    // Display the window
    if(current_screen_status != FULLSCREEN_ONLY)
	{
		ShowWindow(myglobs.hWndMain, SW_SHOWNORMAL);
		UpdateWindow(myglobs.hWndMain);
	}
    // Have the example initialize it components which remain constant
    // throughout execution
    
	
	
	BuildDeviceMenu();

    if (!InitScene())
        return FALSE;
    
	LoadRR_Resources();
	InitRRvariables(&world);

    // Call D3DApp to initialize all DD and D3D objects necessary to render.
    // D3DApp will call the device creation callback which will initialize the
    // viewport and the sample's execute buffers.
    
	strcpy(D3Ddevicename, "display");
	strcpy(D3Ddevicedesc, "Primary display device");
	strcpy(FindD3Ddevicename, "test"); 

    if (!CreateD3DApp(lpCmdLine))	
		return FALSE;
	
	LoadCrossHair();
	LoadBackground();


    return TRUE;
}

BOOL LoadRR_Resources()
{
	int mem;
	char buffer[256];


	mem = sizeof(OBJECTDATA) * MAX_NUM_DAT_OBJECTS;
	mem = mem / 1024;

	itoa(mem, buffer, 10);
	strcat(buffer, " KB");
	PrintMessage(NULL, "memory used by OBJECTDATA array = ", buffer, LOGFILE_ONLY);

	mem = sizeof(PLAYERMODELDATA) * 100;
	mem = mem / 1024;

	itoa(mem, buffer, 10);
	strcat(buffer, " KB");
	PrintMessage(NULL, "memory used by PLAYERMODELDATA array = ", buffer, LOGFILE_ONLY);

	mem = sizeof(world);
	mem = mem / 1024;

	itoa(mem, buffer, 10);
	strcat(buffer, " KB");
	PrintMessage(NULL, "memory used by WORLD array = ", buffer, LOGFILE_ONLY);


	if(!LoadTextures(myglobs.hWndMain, "textures.dat", &world))
	{
		PrintMessage(myglobs.hWndMain, "LoadTextures failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}
	
	if (!LoadObjectData(myglobs.hWndMain,"objects.dat",&world))
	{
		PrintMessage(myglobs.hWndMain, "LoadObjectData failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!LoadImportedModelList(myglobs.hWndMain,"modellist.dat",&world))
	{
		PrintMessage(myglobs.hWndMain, "LoadImportedModelList failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}
	
	if (!LoadWorldMap  (myglobs.hWndMain,"level1.map",&world))
	{
		PrintMessage(myglobs.hWndMain, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!InitPreCompiledWorldMap(myglobs.hWndMain,"level1.cmp", &world))
	{
		PrintMessage(myglobs.hWndMain, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}	

	return TRUE;
}

// CreateD3DApp
// Create all DirectDraw and Direct3D objects necessary to begin rendering.
// Add the list of D3D drivers to the file menu.

BOOL CreateD3DApp(LPSTR lpCmdLine)
{
    HMENU hmenu;
    int i;
    LPSTR option;
    BOOL bOnlySystemMemory, bOnlyEmulation;
    DWORD flags;
    Defaults defaults;

    // Give D3DApp a list of textures to keep track of.
	
	
	
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
    
	if(App_starting_for_first_time == TRUE)
	{
		App_starting_for_first_time = FALSE;

		// Add the the list of display modes D3DApp found to the mode menu
     
		hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 4);
		for (i = 0; i < d3dapp->NumModes; i++) 
		{
			char ach[80];
			wsprintf(ach,"%dx%dx%d", d3dapp->Mode[i].w, d3dapp->Mode[i].h,
			 d3dapp->Mode[i].bpp);
			
			if(d3dapp->Mode[i].bThisDriverCanDo == TRUE)
			AppendMenu(hmenu, MF_STRING, MENU_FIRST_MODE+i, ach);
		}
    
		// Add the list of D3D drivers D3DApp foudn to the file menu
     
	
		hmenu = GetSubMenu(GetSubMenu(GetMenu(myglobs.hWndMain), 0),1);
    
		for (i = 0; i < d3dapp->NumDrivers; i++) 
		{
			AppendMenu(hmenu, MF_BYPOSITION | MF_STRING,
				MENU_FIRST_DRIVER + i, d3dapp->Driver[i].Name);
		}
    }

    // Allow the sample to override the default render state and other
    // settings
     
	
    if (!D3DAppGetRenderState(&defaults.rs)) 
	{
		ReportD3DAppError();
		return FALSE;
    }

    lstrcpy(defaults.Name, "Road Rage 1.1");
    defaults.bTexturesDisabled = FALSE;
    defaults.bResizingDisabled = myglobs.bResizingDisabled;
    defaults.bClearsOn = myglobs.bClearsOn;
    OverrideDefaults(&defaults);

	memcpy(&myglobs.rstate, &defaults.rs, sizeof(D3DAppRenderState));
    
    myglobs.rstate.TextureSize = setupinfo->TextureSize;
	myglobs.rstate.bAlphaTransparency = TRUE;
	myglobs.rstate.bFogEnabled = TRUE;

    // Apply any changes to the render state
    
    
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
	 
		ReleaseView();
	
		// Create the sample's execute buffers via InitView
	 
		if (!InitView(d3dapp->lpDD7, d3dapp->lpD3D, d3dapp->lpD3DDevice,
		      d3dapp->NumUsableTextures)) 
		{
			Msg("InitView failed.\n");
			PrintMessage(myglobs.hWndMain, "CreateD3DApp", NULL, LOGFILE_ONLY);
			CleanUpAndPostQuit();
			return FALSE;
		}
    }

    SetWindowText(myglobs.hWndMain, defaults.Name);
	PrintMessage(myglobs.hWndMain, "CreateD3DApp : suceeded", NULL, LOGFILE_ONLY);
		
	//RRGetCaps();

    return TRUE;
}

/*
 * AfterDeviceCreated
 * D3DApp will call this function immediately after the D3D device has been
 * created (or re-created).  D3DApp expects the D3D viewport to be created and
 * returned.  The sample's execute buffers are also created (or re-created)
 * here.
 */
static BOOL AfterDeviceCreated(int w, int h, LPVOID lpContext)
{
    HRESULT rval;
	DDCOLORKEY ckey;

    // Create the D3D viewport object

	PrintMessage(myglobs.hWndMain, "AfterDeviceCreated : starting", NULL, LOGFILE_ONLY);
		
    // Setup the viewport for a reasonable viewing area
     
    D3DVIEWPORT7 viewData;
    memset(&viewData, 0, sizeof(D3DVIEWPORT7));
    viewData.dwX = viewData.dwY = 0;
    viewData.dwWidth = w;
    viewData.dwHeight = h;
	viewData.dvMinZ = 0.0f;
	viewData.dvMaxZ = 1.0f;

    rval = d3dapp->lpD3DDevice->SetViewport(&viewData);
    if (rval != D3D_OK) 
	{
    	Msg("SetViewport failed.\n%s", D3DAppErrorToString(rval));
		PrintMessage(myglobs.hWndMain, "AfterDeviceCreated 3", NULL, LOGFILE_ONLY);
		CleanUpAndPostQuit();	
		return FALSE;
    }
    PrintMessage(myglobs.hWndMain, "AfterDeviceCreated : SetViewport2 suceeded\n", NULL, LOGFILE_ONLY);
	

    // Create the sample's execute buffers via InitView
     
    if (!InitView(d3dapp->lpDD7, d3dapp->lpD3D, d3dapp->lpD3DDevice,
		  d3dapp->NumUsableTextures))
	{
		Msg("InitView failed.\n");
		PrintMessage(myglobs.hWndMain, "AfterDeviceCreated 4", NULL, LOGFILE_ONLY);
		CleanUpAndPostQuit();	
		return FALSE;
    }
    
    
    // Create and initialize the surfaces containing the frame rate and
    // window information
     
    if (!InitFontAndTextBuffers()) 
	{
		Msg("InitFontAndTextBuffers failed.\n");
		PrintMessage(myglobs.hWndMain, "AfterDeviceCreated 5", NULL, LOGFILE_ONLY);
		CleanUpAndPostQuit();	
		return FALSE;
	}
	ckey.dwColorSpaceLowValue=RGB_MAKE(0,0,0);

	PrintMessage(myglobs.hWndMain, "AfterDeviceCreated : suceeded", NULL, LOGFILE_ONLY);

    return TRUE;
}

// BeforeDeviceDestroyed
// D3DApp will call this function before the current D3D device is destroyed
// to give the app the opportunity to destroy objects it has created with the
// DD or D3D objects.

static BOOL BeforeDeviceDestroyed(LPVOID lpContext)
{
    //HMENU hmenu;
    //int i;
    
    // Release the font object and buffers containing stats
    
    ReleaseFontAndTextBuffers();
        
    
    
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
	HDC hdc;


	hdc = GetDC(myglobs.hWndMain);

	UpdateControls();
		
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

		// Blt the background pic to the backbuffer
		
		d3dapp->lpBackBuffer->Blt(NULL,				// destination
								  lpds_background,	// pointer to background pic surface
								  NULL,				// source
								  DDBLT_ASYNC,
								  NULL);


		// Call the sample's RenderScene to render this frame
		
		
		// init the extent to something wrong
		extents[0].x1 = 2048;
		extents[0].x2 = 0;
		extents[0].y1 = 2048;
		extents[0].y2 = 0;

		if (!RenderScene(d3dapp->lpD3DDevice,
		     &extents[0],cam)) 
		{
			Msg("RenderScene failed.\n");
			return FALSE;
		}
		count = 1;
	
	
		// Blt the frame rate and window stat text to the back buffer
	 
        if (!DisplayFrameRate(&count, &extents[1]))
		    return FALSE;
	
		rc_crosshair.left = 0;
		rc_crosshair.top = 0;
		rc_crosshair.right = 30;
		rc_crosshair.bottom = 30;

		// If in walk mode display crosshair

		if(world.walk_mode_enabled == TRUE)
		{
			d3dapp->lpBackBuffer->BltFast((d3dapp->szClient.cx / 2)-15,
									  (d3dapp->szClient.cy / 2)-15, 
									  lpds_crosshair, 
									  &rc_crosshair,
						 DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		}

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
	
		DisplayScores();


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
		if (!WriteFrameRateBuffer(0.0f, 0, 0))
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



BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
			DisplayCredits(hwnd);
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

	return FALSE;
}




// WindowProc
// Main window message handler.
 
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam,
			   LPARAM lParam )
{	
    int i;
	int idc_device_id;	
	UINT        cmd;

    BOOL bStop;
	BOOL bIsActive;
    LRESULT lresult;
	HINSTANCE hInst;
    
	hInst=myglobs.hInstApp;

    // Give D3DApp an opportunity to process any messages it MUST see in order
    // to perform it's function.
    
    if (!D3DAppWindowProc(&bStop, &lresult, hWnd, message, wParam, lParam)) 
	{
		ReportD3DAppError();
		PrintMessage(myglobs.hWndMain, "WindowProc", NULL, LOGFILE_ONLY);
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
				RRAppActive = TRUE;
			}
			else
				RRAppActive = FALSE;
			break;

		case WM_CREATE:
			SetTimer(hWnd,1,1000,NULL);
			SetTimer(hWnd,2,200,NULL);
			break;
		
		case WM_TIMER: 
			if(wParam == 1)
			{
				KillTimer(hWnd,1);		

				if(current_screen_status == FULLSCREEN_ONLY)
					D3DAppFullscreen(d3dapp->CurrMode);

				if (!D3DAppSetRenderState(&myglobs.rstate)) 
				{
					ReportD3DAppError();
					break;
				}
			
				if(Car_Sound_On_Flag == TRUE)
					InitialiseSound(hWnd);	
			}
			
			if(wParam == 2)
			{
				// Animate players 

				for(i = 0 ; i < world.num_players; i++)
				{			
					mod_id	    = world.player_list[i].model_id;
					curr_frame  = world.player_list[i].current_frame;
					stop_frame  = world.pmdata[mod_id].sequence_stop_frame[world.player_list[i].current_sequence];
					
					if(curr_frame >= stop_frame)
					{	
						world.player_list[i].current_sequence = 0;
						curr_seq = world.player_list[i].current_sequence;
						world.player_list[i].current_frame = world.pmdata[mod_id].sequence_start_frame[curr_seq];
					}
					
					world.player_list[i].current_frame++;
				}

				mod_id	    = world.your_gun[world.current_gun].model_id;
				curr_frame  = world.your_gun[world.current_gun].current_frame;
				stop_frame  = world.pmdata[mod_id].sequence_stop_frame[world.your_gun[world.current_gun].current_sequence];
					
				if(curr_frame >= stop_frame)
				{	
					world.your_gun[world.current_gun].current_sequence = 2;
					curr_seq = world.your_gun[world.current_gun].current_sequence;
					world.your_gun[world.current_gun].current_frame = world.pmdata[mod_id].sequence_start_frame[curr_seq];
				}
				
				world.your_gun[world.current_gun].current_frame++;
				
				// Animate Weapons

				// Animate Your Debug models

			
				for(i = 0 ; i < world.num_debug_models; i++)
				{	
					
					mod_id	    = world.debug[i].model_id;
					curr_frame  = world.debug[i].current_frame;
					stop_frame  = world.pmdata[mod_id].sequence_stop_frame[world.debug[i].current_sequence];
					
					if(curr_frame >= stop_frame)
					{	
						world.debug[i].current_sequence = 0;
						curr_seq = world.debug[i].current_sequence;
						world.debug[i].current_frame = world.pmdata[mod_id].sequence_start_frame[curr_seq];
					}
					else
						world.debug[i].current_frame++;
				}
			
			}
		
			if(wParam == 3)
			{
				KillTimer(myglobs.hWndMain,3);
				delay_key_press_flag = FALSE;
			}

			if(wParam == 5)
			{
				StopDSound(world.your_gun[world.current_gun].sound_id);
				PlayDSound(world.your_gun[world.current_gun].sound_id, NULL);
				SendActionWordMessage(world.current_gun,  APPMSG_FIRE, &DPInfo);
				IsPlayerHit();
				firing_gun_flag = TRUE;
			}
			break;
			
		
		case WM_LBUTTONUP:
			KillTimer(myglobs.hWndMain,5);
			firing_gun_flag = FALSE;
			//firing_gun_flag = FALSE;
			break;

        case WM_LBUTTONDOWN:
			if(world.walk_mode_enabled == TRUE)
			{
				i = world.your_gun[world.current_gun].model_id;
				world.your_gun[world.current_gun].current_sequence = 1; //5
				world.your_gun[world.current_gun].current_frame = world.pmdata[i].sequence_start_frame[world.your_gun[world.current_gun].current_sequence];
				
				StopDSound(world.your_gun[world.current_gun].sound_id);
				PlayDSound(world.your_gun[world.current_gun].sound_id, NULL); 
			
				SendActionWordMessage(world.current_gun,  APPMSG_FIRE, &DPInfo);
				IsPlayerHit();

				SetTimer(myglobs.hWndMain, 5, 75, NULL);
				firing_gun_flag = TRUE;
			}
			break;

		case WM_RBUTTONDOWN:
			i = 0;
			
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
			PrintMessage(myglobs.hWndMain, "WM_DESTROY", NULL, LOGFILE_ONLY);
            CleanUpAndPostQuit();
            break;

		case WM_INITMENUPOPUP:	    
			// Check and enable the appropriate menu items
			CheckMenuItem((HMENU)wParam,ID_LIGHTING_RAMPEFFECT, (cam.ramp_size==16) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam,ID_CONTROL_KEYBOARD,(!Joystick_Enabled_Flag) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam,ID_CONTROL_JOYSTICK,(Joystick_Enabled_Flag) ? MF_CHECKED : MF_UNCHECKED);
		
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
            
			CheckMenuItem((HMENU)wParam, ID_TEXTURES_TEXTURESIZE256X256, (myglobs.rstate.TextureSize == TEXTURESIZE256X256) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_TEXTURES_TEXTURESIZE128X128, (myglobs.rstate.TextureSize == TEXTURESIZE128X128) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_TEXTURES_TEXTURESIZE64X64,	 (myglobs.rstate.TextureSize == TEXTURESIZE64X64)   ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_TEXTURES_ALPHATRANSPARENCY, myglobs.rstate.bAlphaTransparency ? MF_CHECKED : MF_UNCHECKED);

			CheckMenuItem((HMENU)wParam, MENU_FLAT, (myglobs.rstate.ShadeMode == D3DSHADE_FLAT) ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_GOURAUD, (myglobs.rstate.ShadeMode == D3DSHADE_GOURAUD) ? MF_CHECKED : MF_UNCHECKED);
			
			CheckMenuItem((HMENU)wParam, MENU_CLEARS, myglobs.bClearsOn ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, MENU_POINT, (myglobs.rstate.FillMode == D3DFILL_POINT) ? MF_CHECKED : MF_UNCHECKED);
			//CheckMenuItem((HMENU)wParam, MENU_WIREFRAME, (myglobs.rstate.FillMode == D3DFILL_WIREFRAME) ? MF_CHECKED : MF_UNCHECKED);
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
				} 

			case ID_TEXTURES_TEXTURESIZE256X256:
				setupinfo->TextureSize = 256;
				myglobs.rstate.TextureSize = TEXTURESIZE256X256;
				if(d3dappi.bFullscreen == FALSE)
					D3DAppWindow(d3dappi.szClient.cx, d3dappi.szClient.cy);
				else
					D3DAppFullscreen(d3dappi.CurrMode);
				break;

			case ID_TEXTURES_TEXTURESIZE128X128:
				setupinfo->TextureSize = 128;
				myglobs.rstate.TextureSize = TEXTURESIZE128X128;
				if(d3dappi.bFullscreen == FALSE)
					D3DAppWindow(d3dappi.szClient.cx, d3dappi.szClient.cy);
				else
					D3DAppFullscreen(d3dappi.CurrMode);
				break;

			case ID_TEXTURES_TEXTURESIZE64X64:
				setupinfo->TextureSize = 64;
				myglobs.rstate.TextureSize = TEXTURESIZE64X64;
				if(d3dappi.bFullscreen == FALSE)
					D3DAppWindow(d3dappi.szClient.cx, d3dappi.szClient.cy);
				else
					D3DAppFullscreen(d3dappi.CurrMode);
				break;

			case ID_TEXTURES_ALPHATRANSPARENCY:
				myglobs.rstate.bAlphaTransparency = !myglobs.rstate.bAlphaTransparency;
				break;

			case ID_MULTIPLAYER_CONNECT:
				hr = SetupConnection(myglobs.hInstApp, &DPInfo);
				
				if FAILED(hr)
				{
					hr = ShutdownConnection(&DPInfo);
					CoUninitialize();
				}
				else
				{	
					multiplay_flag = TRUE;
					timeSetEvent(50,
					5,
					(LPTIMECALLBACK)UpdatePosCallBack,
					NULL,
					TIME_PERIODIC );
					/*
					timeSetEvent(50,
					10,
					(LPTIMECALLBACK)UpdateControlsCallBack,
					NULL,
					TIME_PERIODIC );
					*/
				}
				break;

			case ID_MULTIPLAYER_DISCONNECT:
				ShutdownConnection(&DPInfo);
				CoUninitialize();
				break;

			case ID_LIGHTING_RAMPEFFECT:
				if(cam.ramp_size==16)
					cam.ramp_size=1;
				else
					cam.ramp_size=16;
				break;

			case ID_CONTROL_KEYBOARD:
				Joystick_Enabled_Flag = FALSE;
				break;
			
			case ID_CONTROL_JOYSTICK:
				Joystick_Enabled_Flag = TRUE;
				break;
					
            case MENU_ABOUT:
                AppPause(TRUE);
                DialogBox(myglobs.hInstApp, "AppAbout", myglobs.hWndMain, (DLGPROC)AppAbout);
                AppPause(FALSE);
                break;
                
			case MENU_EXIT:
				PrintMessage(myglobs.hWndMain, "MENU_EXIT", NULL, LOGFILE_ONLY);
				CleanUpAndPostQuit();
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
						PrintMessage(myglobs.hWndMain, "MENU_FULLSCREEN", NULL, LOGFILE_ONLY);
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
						PrintMessage(myglobs.hWndMain, "MENU_FULLSCREEN 2", NULL, LOGFILE_ONLY);
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
		
		} // end switch WM_COMMAND

		if ((LOWORD(wParam) >= MENU_FIRST_DEVICE) && (LOWORD(wParam) < MENU_FIRST_DEVICE+10))
      {
        //GetMenuString(GetSubMenu(GetSubMenu(GetMenu(myglobs.hWndMain),0), 0), 
		//	0, FindD3Ddevicename, sizeof(FindD3Ddevicename), 
		//	MF_BYPOSITION);

		strcpy(FindD3Ddevicename, D3Ddevicelist[LOWORD(wParam) - MENU_FIRST_DEVICE]);
	
		if(d3dappi.bFullscreen == FALSE)
			D3DAppWindow(d3dappi.szClient.cx, d3dappi.szClient.cy);
		else
			D3DAppFullscreen(d3dappi.CurrMode);

        
      }

		if (   LOWORD(wParam) >= IDC_DEVICES
			&& LOWORD(wParam) < IDC_DEVICES + 99)//g_cpdevFound)
		{
			idc_device_id = (int)LOWORD(wParam)-(int)IDC_DEVICES;
			PickInputDevice(idc_device_id);

			if(idc_device_id == 0)
			{
				Joystick_Enabled_Flag = FALSE;
			}
			else
			{
				Joystick_Enabled_Flag = TRUE;
			}
		}

       
        if (   LOWORD(wParam) >= MENU_FIRST_DRIVER
			&& LOWORD(wParam) < MENU_FIRST_DRIVER + D3DAPP_MAXD3DDRIVERS
			&& d3dapp->CurrDriver != LOWORD(wParam) - MENU_FIRST_DRIVER) 
		{
			// Change the D3D driver
		
			if (!D3DAppChangeDriver(LOWORD(wParam) - MENU_FIRST_DRIVER,NULL)) 
			{
				ReportD3DAppError();
				PrintMessage(myglobs.hWndMain, "D3DAppFullscreen", NULL, LOGFILE_ONLY);
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
				PrintMessage(myglobs.hWndMain, "D3DAppFullscreen", NULL, LOGFILE_ONLY);
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
}

/*
 * CleanUpAndPostQuit
 * Release all D3D objects, post a quit message and set the bQuit flag
 */
void CleanUpAndPostQuit(void)
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
void ReportD3DAppError(void)
{
    Msg("%s", D3DAppLastErrorString());
}

/* Msg
 * Message output for error notification.
 */
void __cdecl Msg( LPSTR fmt, ... )
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

BOOL CALLBACK BuildDeviceMenuCallback(GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam, HMONITOR hm)
{
    HMENU hmenu; 

	hmenu = GetSubMenu((HMENU)lParam, 0);
    AppendMenu(hmenu,MF_STRING,MENU_FIRST_DEVICE + menunumD3Ddevices,szDevice);
	menunumD3Ddevices ++;
	
    return DDENUMRET_OK;
}

void BuildDeviceMenu()
{
    menunumD3Ddevices = 0;
	HMENU hmenu = GetSubMenu(GetMenu(myglobs.hWndMain), 0);
    DirectDrawEnumerateEx(BuildDeviceMenuCallback, (LPVOID)hmenu, DDENUM_ATTACHEDSECONDARYDEVICES );
}

BOOL DDInit()
{
	D3DAppDestroy();
  	//strcpy(D3Ddevicename, szDevice);
	CreateD3DApp(NULL);

    return TRUE;
}


BOOL CALLBACK UpdatePosCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)

{
	int i, angle;

	ReceiveMessage(&DPInfo);
	
	

	if(am_i_the_server == TRUE)
	{
		for(i = 0; i < world.num_players; i++)
		{	
			angle = (int)-world.player_list[i].rot_angle + 360; 
			angle = angle + 90;

			if(angle >= 360)
				angle = angle - 360;
			if(angle < 0)
				angle = angle + 360;

			SendPositionMessage(world.player_list[i].RRnetID, 
								world.player_list[i].x, 
								world.player_list[i].y, 
								world.player_list[i].z, 
								(float)angle, 
								&DPInfo);
		}
	}
	else
	{
		angle = -cam.view_angle + 360; 
		angle = angle + 90;

		if(angle >= 360)
			angle = angle - 360;
		if(angle < 0)
			angle = angle + 360;

		SendPositionMessage(MyRRnetID, 
							cam.view_point_x, 
							cam.view_point_y, 
							cam.view_point_z, 
							(float)angle, 
							&DPInfo);
	}
	return TRUE;
}

BOOL CALLBACK UpdateControlsCallBack(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)

{
	UpdateControls();

	return TRUE;
}

void IsPlayerHit()
{
	int i;
	int num_hits = 0;

	for(i = 0; i < 2; i++)
	{
		gv_dx = world.player_list[i].x - cam.view_point_x;
		gv_dz = world.player_list[i].z - cam.view_point_z;

		gv_gradient		= gv_dz / gv_dx; 
		gv_target_angle = -1 * ((float)180 / (float)3.14159) * (float)atan(gv_gradient);
		gv_distance		= (float)sqrt(gv_dx * gv_dx + gv_dz * gv_dz);
	
		gv_temp_angle	= (float)cam.view_angle;
	
		if((gv_dx >= 0) && (gv_dz >= 0))
		{
			gv_quadrant = 0; //quadrant 1    0 to 90
			gv_target_angle = 90 + gv_target_angle; 
		}

		if((gv_dx >= 0) && (gv_dz < 0))
			gv_quadrant = 90; //quadrant 2   90 to 180 

		if((gv_dx < 0) && (gv_dz < 0))
		{
			gv_quadrant = 180; //quadrant 3  180 to 270
			gv_target_angle = 80 + gv_target_angle; 
		}

		if((gv_dx < 0) && (gv_dz >= 0))
			gv_quadrant = 270; //quadrant 4  270 to 360
		
		gv_target_angle += gv_quadrant;


		if (gv_temp_angle >= 360)
			gv_temp_angle = gv_temp_angle - 360;

		if (gv_temp_angle < 0)
			gv_temp_angle = gv_temp_angle + 360;


		if (gv_target_angle >= 360)
			gv_target_angle = gv_target_angle - 360;

		if (gv_target_angle < 0)
			gv_target_angle = gv_target_angle + 360;

		gv_da			= gv_temp_angle - gv_target_angle;
		gv_hit_width	= (float)(abs((int)((float)tan(k * gv_da) * gv_distance)));
	
		itoa((int)gv_target_angle, buffer2, 10);
		itoa((int)gv_temp_angle, buffer3, 10);
		itoa((int)gv_quadrant, buffer4, 10);
		strcat(buffer2, " : ");
		strcat(buffer2, buffer3); 
		strcat(buffer2, " : ");
		strcat(buffer2, buffer4); 

		if(gv_hit_width < 10)
		{
			mod_id = world.player_list[i].model_id;
			world.player_list[i].current_sequence = 3;
			world.player_list[i].current_frame = world.pmdata[mod_id].sequence_start_frame[world.player_list[i].current_sequence];
				
			strcpy(buffer, "Hit ! ");
			strcat(buffer, buffer2);
			DDTextOut(buffer);
			if(multiplay_flag == TRUE)
				SendActionWordMessage(i,  APPMSG_HIT, &DPInfo);
			num_hits++;
		}
			
	} // end for i loop	
}


void LoadCrossHair()
{
	DDCOLORKEY ckey;
	DDSURFACEDESC2 ddsd;



	memcpy(&ddsd, &d3dapp->ThisTextureFormat.ddsd, sizeof(DDSURFACEDESC2));
	memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ddpfBackBuffer, sizeof(DDPIXELFORMAT));

	// load background PPM file

	lpds_crosshair = D3DAppILoadSurface(d3dapp->lpDD7, "crosshair.ppm",
		    &ddsd, DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);

	if (lpds_crosshair == NULL) 
	{	
		PrintMessage(myglobs.hWndMain, "crosshair : FAILED to create system memory surface for background", NULL, SCN_AND_FILE);
	

		lpds_crosshair = D3DAppILoadSurface(d3dapp->lpDD7, "crosshair.ppm",
		    &ddsd, DDSCAPS_VIDEOMEMORY);
		if (lpds_crosshair == NULL) 
		{	
			PrintMessage(myglobs.hWndMain, "crosshair: D3DAppILoadSurface FAILED to create video memory surface for background", NULL, SCN_AND_FILE);
			return;
		}
	}

	ckey.dwColorSpaceLowValue = RGB_MAKE(0,0,0);
	
	lpds_crosshair->SetColorKey(DDCKEY_SRCBLT,&ckey);

	PrintMessage(myglobs.hWndMain, "LoadCrossHair : suceeded", NULL, LOGFILE_ONLY);

}

void LoadBackground()
{
	DDSURFACEDESC2 ddsd;

	// load ddsd with the back buffer's pixel format for creating 
	// off screen surfaces,that contain images that will be blited to the 
	// back buffer later on. Note the D3D textures may have a different
	// pixel format from the back buffers. 
	// i.e if the back buffer is in RGB 555 mode and your D3D textures
	// are in 565 that's ok, but if you want to DD blit an image to the 
	// backbuffer, then you have to create a DD surface for the image that has
	// the same pixel format as the backbuffer. Otherwise you may end up
	// with blit images that are abnormally green or yellow.

	memcpy(&ddsd, &d3dapp->ThisTextureFormat.ddsd, sizeof(DDSURFACEDESC2));

	memcpy(&ddsd.ddpfPixelFormat, &d3dappi.ddpfBackBuffer, sizeof(DDPIXELFORMAT));

	// load background PPM file

	lpds_background = D3DAppILoadSurface(d3dapp->lpDD7, "backgnd.ppm",
		    &ddsd, DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY);
	if (lpds_background == NULL) 
	{	
		PrintMessage(myglobs.hWndMain, "LoadBackground : FAILED to create system memory surface for background", NULL, SCN_AND_FILE);
	

		lpds_background = D3DAppILoadSurface(d3dapp->lpDD7, "backgnd.ppm",
		    &ddsd, DDSCAPS_VIDEOMEMORY);
		if (lpds_background == NULL) 
		{	
			PrintMessage(myglobs.hWndMain, "LoadBackground : D3DAppILoadSurface FAILED to create video memory surface for background", NULL, SCN_AND_FILE);
		}
	}
	PrintMessage(myglobs.hWndMain, "LoadBackground : suceeded", NULL, LOGFILE_ONLY);

}

void DisplayScores()
{
	int i;
	HDC hdc;
	char buf[256];

	if(world.display_scores == FALSE)
		return;

	LastError = d3dapp->lpBackBuffer->GetDC(&hdc);
	if (LastError == DD_OK) 
	{	
		SetTextColor(hdc, RGB(255,255,0));
		SetBkColor(hdc, RGB(0,0,0));
		SetBkMode(hdc, TRANSPARENT);
	
		TextOut(hdc,  50, 20, "Scores", 6);
		TextOut(hdc,  50, 40, "No.", 3);
		TextOut(hdc,  90, 40, "Name", 4);
		TextOut(hdc, 250, 40, "Frags", 5);
		TextOut(hdc, 350, 40, "dpip", 4);
		TextOut(hdc, 500, 40, "RRnetID", 7);
		
		for(i = 50; i < 300; i+=10)
			TextOut(hdc, i, 56, "--", 2);


		for(i=0; i<world.num_players;i++)
		{
			itoa(i,buf,10);
			TextOut(hdc, 50, 80+i*20, buf, strlen(buf));

			strcpy(buf, world.player_list[i].name);
			TextOut(hdc, 90, 80+i*20, buf, strlen(buf));
			
			itoa(world.player_list[i].frags, buf,10);
			TextOut(hdc, 250, 80+i*20, buf,strlen(buf));

			itoa((int)world.player_list[i].dpid, buf,10);
			TextOut(hdc, 350, 80+i*20, buf,strlen(buf));

			itoa((int)world.player_list[i].RRnetID, buf,10);
			TextOut(hdc, 500, 80+i*20, buf,strlen(buf));
			
		}
		d3dapp->lpBackBuffer->ReleaseDC(hdc);
	}

}