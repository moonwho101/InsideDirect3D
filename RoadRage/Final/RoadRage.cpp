//-----------------------------------------------------------------------------
// File: RoadRage.cpp
//
// Desc: The main RoadRage code.  The CMyD3DApplication class handles
//		 most of the RoadRage functionality.
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#define D3D_OVERLOADS
#include "resource.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <dinput.h>
#include <dplobby.h>
#include <dplay.h>
#include "D3DApp.h"
#include <dsound.h>
#include "D3DUtil.h"
#include "D3DEnum.h"
#include "world.hpp"
#include "3Dengine.hpp"
#include "RoadRage.hpp"
#include "D3DTextr.h" 
#include "ImportMD2.hpp"
#include "Import3DS.hpp"
#include "LoadWorld.hpp"
#include "DpMessages.hpp"
#include "dplay2.hpp"
#include <tchar.h>

#define SND_FIRE_AR15  0
#define SND_FIRE_MP5   1

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define MAX_SOUND_BUFFERS 30
#define MAX_PLAYER_NAME      14
#define MAX_SESSION_NAME     256
#define MAX_NUM_OBJECTS_PER_CELL 50
#define DPLAY_SAMPLE_KEY     TEXT("Software\\Microsoft\\DirectX DirectPlay Samples")

#define EXITCODE_FORWARD     1  // Dialog success, so go forward
#define EXITCODE_BACKUP      2  // Dialog canceled, show previous dialog
#define EXITCODE_QUIT        3  // Dialog quit, close app
#define EXITCODE_ERROR       4  // Dialog error, terminate app

// App specific DirectPlay messages and structures 
//-----------------------------------------------------------------------------
#define GAMEMSG_WAVE        1

enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOSOFTWARE };

extern BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM );
extern HRESULT         DPConnect_CheckForLobbyLaunch( BOOL* pbLaunchedByLobby );
extern int             DPConnect_StartDirectPlayConnect( HINSTANCE hInst, BOOL bBackTrack );
extern HANDLE               g_hDPMessageEvent;
extern LPDIRECTSOUNDBUFFER g_pDSBuffer[MAX_SOUND_BUFFERS];

FLOAT                g_fCurrentTime;
extern LPDIRECTPLAY4A       g_pDP;
extern LPDPLCONNECTION      g_pDPLConnection;
extern LPDIRECTPLAYLOBBY3   g_pDPLobby;
extern GUID                 g_AppGUID; 
extern HANDLE               g_hDPMessageEvent;
extern HKEY               g_hDPlaySampleRegKey;                      
extern TCHAR                g_strPreferredProvider[MAX_SESSION_NAME];
extern TCHAR                g_strSessionName[MAX_SESSION_NAME];
extern TCHAR                g_strLocalPlayerName[MAX_PLAYER_NAME];
extern DPID                 g_LocalPlayerDPID; 
extern BOOL                 g_bHostPlayer;
extern TCHAR                g_strAppName[256];
extern BOOL                 g_bUseProtocol;
extern VOID*              g_pvDPMsgBuffer;                          
extern DWORD              g_dwDPMsgBufferSize;                  
extern DWORD              g_dwNumberOfActivePlayers;
		
extern LPDIRECTINPUTDEVICE2 g_Keyboard_pdidDevice2;
extern LPDIRECTINPUT7       g_Keyboard_pDI;
extern CMD2* pCMD2;
extern C3DS* pC3DS;
extern char rr_multiplay_chat_string[1024];
extern DPLAYINFO DPInfo;

LPDIRECTINPUT7       g_pDI;            // The DInput object
LPDIRECTINPUT7       g_Keyboard_pDI;            // The DInput object
LPDIRECTINPUTDEVICE2 g_pdidDevice2;    // The DIDevice2 interface
LPDIRECTINPUTDEVICE2 g_Keyboard_pdidDevice2;    // The DIDevice2 interface
GUID                 g_guidJoystick;   // The GUID for the joystick

BOOL g_bUseKeyboard = TRUE;
BOOL g_bUseMouse    = FALSE;
BOOL g_bUseJoystick = FALSE;
BOOL bMouseLookOn   = TRUE;
BOOL bMouseLookup_is_mouse_forward = TRUE;

BOOL bIsFlashlightOn    = FALSE;
BOOL logfile_start_flag = TRUE;
BOOL RR_exit_debug_flag = FALSE;
SETUPINFO setupinfo;
HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;
int num_tex;

int num_op_guns = 0;
int num_cars;

int total_allocated_memory_count = 0;
int num_light_sources;
float angle_deg = 0.0f; // debug
float temp_f	= 0.0f; // debug

float weapon_flash_r;
float weapon_flash_g;

CONTROLS Controls;

CMyD3DApplication* pCMyApp;
extern CLoadWorld* pCWorld;

void PrintMemAllocated(int mem, char *message);
VOID DisplayError( CHAR* strMessage );

BOOL bSkipThisCell;


//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM )
{
    if( WM_COMMAND == uMsg )
        if( IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam) )
            EndDialog (hWnd, TRUE);

    return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()                
{
    m_strWindowTitle  = TEXT( "Road Rage" );
    m_bAppUseZBuffer  = TRUE;
    m_fnConfirmDevice = NULL;

    m_dltType = D3DLIGHT_SPOT;
	k = (float)0.017453292;
	
	m_vUpVec = D3DVECTOR(0, 1, 0);
	m_tableFog = FALSE;

	rendering_first_frame = TRUE;
	RRAppActive			  = FALSE;
	firing_gun_flag		  = FALSE;
	delay_key_press_flag  = FALSE;
	multiplay_flag		  = FALSE;
	display_scores		  = FALSE;
	bInWalkMode			  = TRUE;
	bInTalkMode			  = FALSE;
	num_packets_sent	 = 0;
	num_packets_received = 0;
    m_bShowStats      = FALSE;
    m_bWindowed    = TRUE;
    m_bToggleFullscreen = FALSE;
    m_bUseViewPoints = FALSE;

	m_toggleFog		  = FALSE;
	m_num_sounds = 0;
	num_players			= 0;
	num_your_guns		= 0;
	num_cars			= 0;
	num_debug_models	= 0;
	current_frame		= 0;
	current_sequence	= 0;
	current_frame		= 0;		
	current_sequence	= 0;

	car_gear			= 1;
	editor_insert_mode  = 0;
	object_rot_angle	= 0;	
	toggle_delay		= 0;

	tx_rate_ps = 50;
	rx_rate_ps = 50;

	current_screen_status = FULLSCREEN_ONLY; // default, in case rrsetup.ini 
	
	num_packets_received_ps = 0;
	num_packets_sent_ps = 0;

	pi = 3.141592654f;
	piover2 = (pi / 2.0f);

	Z_NEAR = D3DVAL(6.0);
	Z_FAR = D3DVAL(10000.0);
	FOV = D3DVAL(pi / 2.0f); // Field Of View set to 90 degrees 

    // Move the camera position around
    D3DVECTOR vFrom( 700.0f, 22.0f, 700.0f );
    D3DVECTOR vAt(850.0f, 25.0f, 480.0f);
    D3DVECTOR vUp(0.0f, 1.0f, 0.0f);
    SetViewParams( &vFrom, &vAt, &vUp, 0.1f);

	IsRenderingOk = FALSE;
	bEnableAlphaTransparency = FALSE;
	bEnableLighting = TRUE;
	ShadeMode = D3DSHADE_GOURAUD;


	pCMyApp = this;
}


//-----------------------------------------------------------------------------
// Name: OutputText()
// Desc: Draws text on the window.
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::OutputText( DWORD x, DWORD y, TCHAR* str )
{
    HDC hDC;

    // Get a DC for the surface. Then, write out the buffer
    if( m_pddsRenderTarget )
    {
        if( SUCCEEDED( m_pddsRenderTarget->GetDC(&hDC) ) )
        {
            SetTextColor( hDC, RGB(255,255,0) );
            SetBkMode( hDC, TRANSPARENT );
            ExtTextOut( hDC, x, y, 0, NULL, str, lstrlen(str), NULL );
            m_pddsRenderTarget->ReleaseDC(hDC);
        }
    }

    // Do the same for the left surface (in case of stereoscopic viewing).
    if( m_pddsRenderTargetLeft )
    {
        if( SUCCEEDED( m_pddsRenderTargetLeft->GetDC( &hDC ) ) )
        {
            // Use a different color to help distinguish left eye view
            SetTextColor( hDC, RGB(255,0,255) );
            SetBkMode( hDC, TRANSPARENT );
            ExtTextOut( hDC, x, y, 0, NULL, str, lstrlen(str), NULL );
            m_pddsRenderTargetLeft->ReleaseDC(hDC);
        }
    }
}


//-----------------------------------------------------------------------------
// Name: ShowStats()
// Desc: Shows frame rate and dimensions of the rendering device.
//-----------------------------------------------------------------------------

VOID CMyD3DApplication::ShowStats()
{
    static FLOAT fFPS      = 0.0f;
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames  = 0L;

    // Keep track of the time lapse and frame count
    FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
    ++dwFrames;

    // Update the frame rate once per second
    if( fTime - fLastTime > 1.0f )
    {
        fFPS      = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames  = 0L;

		num_packets_received_ps = num_packets_received;
		num_packets_sent_ps	 = num_packets_sent;
		num_packets_received = 0;
		num_packets_sent     = 0;

		SendActionFloatMessage(MyRRnetID, 
							   fTime,  
							   APPMSG_PING_TO_SERVER, 
							   DPID_SERVERPLAYER);
							   
    }

    // Setup the text buffer to write out dimensions
    CHAR           buffer[80];
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    m_pddsRenderTarget->GetSurfaceDesc( &ddsd );

    sprintf( buffer, "%7.02f fps (%dx%dx%d)", fFPS, ddsd.dwWidth,
             ddsd.dwHeight, ddsd.ddpfPixelFormat.dwRGBBitCount );
    OutputText( 0, 0, buffer );

	sprintf( buffer, "ping : %d", MyPing);
	OutputText( 200, 0, buffer );

	sprintf( buffer, "health : %d", MyHealth);
	OutputText( 300, 0, buffer );
}


//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated joystick. If we find one, 
//       create a device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumJoysticksCallback( LPCDIDEVICEINSTANCE pInst, 
                                     VOID* pvContext )
{
    memcpy( pvContext, &pInst->guidInstance, sizeof(GUID) );

    return DIENUM_STOP;
}


//-----------------------------------------------------------------------------
// Name: InputDeviceSelectProc()
// Desc: Dialog procedure for selecting an input device.
//-----------------------------------------------------------------------------
BOOL CALLBACK InputDeviceSelectProc( HWND hWnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam )
{
    HWND hwndKeyboardButton = GetDlgItem( hWnd, IDC_KEYBOARD );
    HWND hwndMouseButton    = GetDlgItem( hWnd, IDC_MOUSE );
    HWND hwndJoystickButton = GetDlgItem( hWnd, IDC_JOYSTICK );

    if( WM_INITDIALOG == msg )
    {
        SendMessage( hwndKeyboardButton, BM_SETCHECK, g_bUseKeyboard, 0L);
        SendMessage( hwndMouseButton,    BM_SETCHECK, g_bUseMouse,    0L);
        SendMessage( hwndJoystickButton, BM_SETCHECK, g_bUseJoystick, 0L);
        
        EnableWindow( hwndJoystickButton, (g_guidJoystick != GUID_NULL) );

        return TRUE;
    }

    if( WM_COMMAND == msg && IDOK == LOWORD(wParam) )
    {
        // Destroy the old device
        pCMyApp->DestroyInputDevice();

        // Check the dialog controls to see which device type to create
        g_bUseKeyboard = SendMessage( hwndKeyboardButton, BM_GETCHECK, 0, 0L);
        g_bUseMouse    = SendMessage( hwndMouseButton,    BM_GETCHECK, 0, 0L);
        g_bUseJoystick = SendMessage( hwndJoystickButton, BM_GETCHECK, 0, 0L);
    
        if( g_bUseKeyboard )
        {
            pCMyApp->CreateInputDevice( GetParent(hWnd), g_Keyboard_pDI, 
									g_Keyboard_pdidDevice2,
									GUID_SysKeyboard, &c_dfDIKeyboard, 
									DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );               
			g_Keyboard_pdidDevice2->Acquire();
        }
			
        if( g_bUseMouse ) 
        {
            pCMyApp->CreateInputDevice( GetParent(hWnd), g_pDI, 
									g_pdidDevice2,
									GUID_SysMouse, &c_dfDIMouse, 
									DISCL_EXCLUSIVE|DISCL_FOREGROUND );                               
			g_pdidDevice2->Acquire();

			pCMyApp->CreateInputDevice( GetParent(hWnd), g_Keyboard_pDI, 
									g_Keyboard_pdidDevice2,
									GUID_SysKeyboard, &c_dfDIKeyboard, 
									DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );               
			g_Keyboard_pdidDevice2->Acquire();
        }
		
        if( g_bUseJoystick ) 
        {
            pCMyApp->CreateInputDevice( GetParent(hWnd), g_pDI, 
									g_pdidDevice2,
									g_guidJoystick, &c_dfDIJoystick, 
									DISCL_EXCLUSIVE|DISCL_FOREGROUND );
			g_pdidDevice2->Acquire();


            // Set the range of the joystick axes tp [-1000,+1000]
            DIPROPRANGE diprg; 
            diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
            diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            diprg.diph.dwHow        = DIPH_BYOFFSET; 
            diprg.lMin              = -10; 
            diprg.lMax              = +10; 

            diprg.diph.dwObj = DIJOFS_X;    // Set the x-axis range
            g_pdidDevice2->SetProperty( DIPROP_RANGE, &diprg.diph );

            diprg.diph.dwObj = DIJOFS_Y;    // Set the y-axis range
            g_pdidDevice2->SetProperty( DIPROP_RANGE, &diprg.diph );

            // Set the dead zone for the joystick axes (because many joysticks
            // aren't perfectly calibrated to be zero when centered).
            DIPROPDWORD dipdw; 
            dipdw.diph.dwSize       = sizeof(DIPROPDWORD); 
            dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
            dipdw.diph.dwHow        = DIPH_DEVICE; 
            dipdw.dwData            = 1000; // Here, 1000 = 10%

            dipdw.diph.dwObj = DIJOFS_X;    // Set the x-axis deadzone
            g_pdidDevice2->SetProperty( DIPROP_DEADZONE, &dipdw.diph );

            dipdw.diph.dwObj = DIJOFS_Y;    // Set the y-axis deadzone
            g_pdidDevice2->SetProperty( DIPROP_RANGE, &dipdw.diph );
        }

        EndDialog( hWnd, IDOK );
        return TRUE;
    }

    if( WM_COMMAND == msg && IDCANCEL == LOWORD(wParam) )
    {
        EndDialog( hWnd, IDCANCEL );
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// Name: DisplayError()
// Desc: Displays an error message.
//-----------------------------------------------------------------------------
VOID DisplayError( CHAR* strMessage )
{
    MessageBox( NULL, strMessage, "DInput Sample Game", MB_OK );
}

//-----------------------------------------------------------------------------
// Name: CreateDInput()
// Desc: Initialize the DirectInput objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateDInput( HWND hWnd )
{
    // Create the main DirectInput object

	PrintMessage(NULL, "CD3DApplication::CreateDInput()",NULL, LOGFILE_ONLY);
    
    if( FAILED( DirectInputCreateEx( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
                                   DIRECTINPUT_VERSION, IID_IDirectInput7, (LPVOID*) &g_pDI, NULL) ) )
    {
		DisplayError( "DirectInputCreate() failed." );
        return E_FAIL;
    }

    // Check to see if a joystick is present. If so, the enumeration callback
    // will save the joystick's GUID, so we can create it later.
    ZeroMemory( &g_guidJoystick, sizeof(GUID) );
    
    g_pDI->EnumDevices( DIDEVTYPE_JOYSTICK, EnumJoysticksCallback,
                        &g_guidJoystick, DIEDFL_ATTACHEDONLY );

	// keyboard

	if( FAILED( DirectInputCreateEx( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
                                   DIRECTINPUT_VERSION, IID_IDirectInput7, (LPVOID*) &g_Keyboard_pDI, NULL) ) )
    {
        DisplayError( "DirectInputCreate() failed." );
        return E_FAIL;
    }
	
    return S_OK;
}






//-----------------------------------------------------------------------------
// Name: DestroyDInput()
// Desc: Terminate our usage of DirectInput
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DestroyDInput()
{
    // Destroy the DInput object
    if( g_pDI )
        g_pDI->Release();
    g_pDI = NULL;
}


//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateInputDevice( HWND hWnd, 
								   LPDIRECTINPUT7       pDI,	
								   LPDIRECTINPUTDEVICE2 pDIdDevice, 
								   GUID guidDevice,
                           const DIDATAFORMAT* pdidDataFormat, DWORD dwFlags )
{

	PrintMessage(NULL, "CD3DApplication::CreateInputDevice()",NULL, LOGFILE_ONLY);

    // Obtain an interface to the input device
    if( FAILED( pDI->CreateDeviceEx( guidDevice, IID_IDirectInputDevice2,
		                               (VOID**)&pDIdDevice, NULL ) ) )
    {
		PrintMessage(NULL, "CD3DApplication::CreateInputDevice() - CreateDeviceEx FAILED",NULL, LOGFILE_ONLY);
        DisplayError( "CreateDeviceEx() failed" );
        return E_FAIL;
    }
	else
		PrintMessage(NULL, "CD3DApplication::CreateInputDevice() - CreateDeviceEx ok",NULL, LOGFILE_ONLY);


    // Set the device data format. Note: a data format specifies which
    // controls on a device we are interested in, and how they should be
    // reported.
    if( FAILED( pDIdDevice->SetDataFormat( pdidDataFormat ) ) )
    {
        DisplayError( "SetDataFormat() failed" );
        return E_FAIL;
    }

    // Set the cooperativity level to let DirectInput know how this device
    // should interact with the system and with other DirectInput applications.
    if( FAILED( pDIdDevice->SetCooperativeLevel( hWnd, dwFlags ) ) )
    {
        DisplayError( "SetCooperativeLevel() failed" );
        return E_FAIL;
    }

	if(guidDevice == GUID_SysKeyboard)
		g_Keyboard_pdidDevice2 = pDIdDevice;
	else
		g_pdidDevice2 = pDIdDevice;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DestroyInputDevice()
// Desc: Release the DirectInput device
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::DestroyInputDevice()
{
    // Unacquire and release the device's interfaces
    if( g_pdidDevice2 )
    {
        g_pdidDevice2->Unacquire();
        g_pdidDevice2->Release();
        g_pdidDevice2 = NULL;
    }

	// keyboard

	if( g_Keyboard_pdidDevice2 )
    {
        g_Keyboard_pdidDevice2->Unacquire();
        g_Keyboard_pdidDevice2->Release();
        g_Keyboard_pdidDevice2 = NULL;
    }
	
        
}


VOID CMyD3DApplication::UpdateControls()
{
	int i;
	int look_up = 0;
	int look_down = 0;


  // Get the relative time, in seconds
    FLOAT fTime = ( timeGetTime() - GetBaseTime() ) * 0.001f;

	g_fCurrentTime = timeGetTime() * 0.001f;

    if( (g_Keyboard_pdidDevice2) || (g_pdidDevice2) )
    {
        HRESULT      hr;
        BYTE         diks[256]; // DInput keyboard state buffer
        DIMOUSESTATE dims;      // DInput mouse state structure
        DIJOYSTATE   dijs;      // DInput joystick state structure

        // read the current keyboard state
		if( g_bUseKeyboard )
		{
			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState( sizeof(diks), &diks );
		}

		// read the current mouse and keyboard state
		if( g_bUseMouse )
		{
			g_Keyboard_pdidDevice2->Acquire();
			hr = g_Keyboard_pdidDevice2->GetDeviceState( sizeof(diks), &diks );

			g_pdidDevice2->Acquire();	
            hr = g_pdidDevice2->GetDeviceState( sizeof(DIMOUSESTATE),&dims );	
		}

		// read the current joystick state
        if( g_bUseJoystick ) 
		{
			// Poll the device before reading the current state. This is required
			// for some devices (joysticks) but has no effect for others (keyboard
			// and mice). Note: this uses a DIDevice2 interface for the device.
        
			g_pdidDevice2->Poll();

			g_pdidDevice2->Acquire();	
			hr = g_pdidDevice2->GetDeviceState( sizeof(DIJOYSTATE),&dijs );

		}
        			
        // Check whether the input stream has been interrupted. If so,
        // re-acquire and try again.
        if( hr == DIERR_INPUTLOST )
        {
			PrintMessage(NULL, "DIERR_INPUTLOST", NULL, LOGFILE_ONLY);
                
            hr = g_pdidDevice2->Acquire();
            if( FAILED(hr) )
			{
				PrintMessage(NULL, "Acquire Input device FAILED", NULL, LOGFILE_ONLY);
                return;// S_OK;
			}
        }
 
        // Read Keyboard input only
		if( g_bUseKeyboard )
        {
			Controls.bLeft		= diks[DIK_LEFT]	&& 0x80;
			Controls.bRight		= diks[DIK_RIGHT]	&& 0x80;
			Controls.bForward	= diks[DIK_UP]		&& 0x80;
			Controls.bBackward	= diks[DIK_DOWN]	&& 0x80;
			Controls.bUp		= diks[DIK_NUMPADPLUS]	&& 0x80;
			Controls.bDown		= diks[DIK_NUMPADMINUS]	&& 0x80;
			Controls.bHeadUp	= diks[DIK_PGUP]	&& 0x80;
			Controls.bHeadDown	= diks[DIK_PGDN]	&& 0x80;
			Controls.bStepLeft	= diks[DIK_COMMA]	&& 0x80;
			Controls.bStepRight = diks[DIK_PERIOD]	&& 0x80;
			Controls.bFire		= diks[DIK_LCONTROL]&& 0x80;
			Controls.bScores	= diks[DIK_S]		&& 0x80;
			Controls.bPrevWeap	= diks[DIK_INSERT]	&& 0x80;
			Controls.bNextWeap	= diks[DIK_DELETE]	&& 0x80;
			Controls.bTravelMode= diks[DIK_SPACE]	&& 0x80;
			Controls.bInTalkMode= diks[DIK_TAB]   	&& 0x80;
		}

        // Read mouse and keyboard input
        if( g_bUseMouse )
        {
            // Note: on really fast computers, the mouse will appear to be 
            // still most of the time, and move in jumps. To combat this, we'll
            // keep 0.1 seconds of persistence for any up/down values we read.
            static FLOAT fUpTime = 0.0f;
            static FLOAT fDnTime = 0.0f;
            if( dims.lY < 0 ) fDnTime = 0.0f, fUpTime = g_fCurrentTime+0.1f;
            if( dims.lY > 0 ) fUpTime = 0.0f, fDnTime = g_fCurrentTime+0.1f;

			if(bMouseLookOn == TRUE )
			{
				if(bMouseLookup_is_mouse_forward == TRUE)
				{
					look_up   = fDnTime-g_fCurrentTime > 0.0f;
					look_down = fUpTime-g_fCurrentTime > 0.0f;
				}
				else
				{
					look_down   = fDnTime-g_fCurrentTime > 0.0f;
					look_up		= fUpTime-g_fCurrentTime > 0.0f;
				}
			}

			Controls.bLeft		= dims.lX<0;
			Controls.bRight		= dims.lX>0;
			Controls.bForward	= diks[DIK_UP]		&& 0x80;
			Controls.bBackward	= diks[DIK_DOWN]	&& 0x80;
			Controls.bUp		= diks[DIK_ADD]		&& 0x80;
			Controls.bDown		= diks[DIK_SUBTRACT]	&& 0x80;
			Controls.bHeadUp	= look_up;
			Controls.bHeadDown	= look_down;
			Controls.bStepLeft	= diks[DIK_COMMA]	&& 0x80;
			Controls.bStepRight = diks[DIK_PERIOD]	&& 0x80;
			Controls.bFire		= dims.rgbButtons[0] && 0x80;
			Controls.bScores	= diks[DIK_S]		&& 0x80;
			Controls.bPrevWeap	= diks[DIK_INSERT]	&& 0x80;
			Controls.bNextWeap	= diks[DIK_DELETE]	&& 0x80;
			Controls.bTravelMode= diks[DIK_SPACE]	&& 0x80;
        }

        // Read joystick input
        if( g_bUseJoystick )
        {
			Controls.bLeft		= dijs.lX<0;
			Controls.bRight		= dijs.lX>0;
			Controls.bForward	= dijs.lY<0;
			Controls.bBackward	= dijs.lY>0;
			Controls.bUp		= diks[DIK_ADD]		&& 0x80;
			Controls.bDown		= diks[DIK_SUBTRACT]&& 0x80;
			Controls.bHeadUp	= look_up;
			Controls.bHeadDown	= look_down;
			Controls.bStepLeft	= diks[DIK_COMMA]	&& 0x80;
			Controls.bStepRight = diks[DIK_PERIOD]	&& 0x80;
			Controls.bFire		= dijs.rgbButtons[0]&& 0x80;
			Controls.bScores	= diks[DIK_S]		&& 0x80;
			Controls.bPrevWeap	= diks[DIK_INSERT]	&& 0x80;
			Controls.bNextWeap	= diks[DIK_DELETE]	&& 0x80;
			Controls.bTravelMode= diks[DIK_SPACE]	&& 0x80;	
        }

		for(i = 0; i < 256; i++)
		{
			if( (diks[i] && 0x80) == FALSE) 
				DelayKey2[i] = FALSE;
		}

		if((Controls.bInTalkMode == TRUE) && (DelayKey2[DIK_TAB] == FALSE))
		{
			bInTalkMode = !bInTalkMode;
			ResetChatString();
			DelayKey2[DIK_TAB] = TRUE;
		}

		if(RRAppActive == TRUE)
		{
			if(bInTalkMode == FALSE)
				MovePlayer(&Controls);
			else
				HandleTalkMode(diks);
		}
		
    }

}

//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::Cleanup3DEnvironment()
{
    SetbActive(FALSE);
    SetbReady(FALSE);

    if( GetFramework() )
    {
        DeleteDeviceObjects();
        DeleteFramework();

        FinalCleanup();
    }

    D3DEnum_FreeResources();
}


LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam )
{
	int MyPlayerNum;
	int     nExitCode;
    BOOL    bLaunchedByLobby;
	float fStart;
	float fEnd;
	float fDensity = 1.0;
	BOOL fUseRange = TRUE;
	HRESULT hr;
	HMENU hMenu;

	m_hWnd = hWnd;
		
	hMenu = GetMenu( hWnd );

    switch( uMsg )
    {
		case WM_ACTIVATEAPP:
			RRAppActive = (BOOL)wParam;
            break;

		case WM_CREATE:
			SetTimer(hWnd, ANIMATION_TIMER, 250,NULL);
			break;
		
		case WM_TIMER: 
			
			if(wParam == ANIMATION_TIMER)
			{
				if( GetFramework() && GetbActive() && GetbReady() && m_bWindowed )
				{
					if(IsRenderingOk == TRUE)
						AnimateCharacters();	
				}
			}

			if(wParam == RESPAWN_TIMER)
			{
				m_vEyePt.x = 700;
				m_vEyePt.y = 22;
				m_vEyePt.z = 700;

				MyHealth = 100;

				MyPlayerNum = GetPlayerNumber(MyRRnetID);

				player_list[MyPlayerNum].x = m_vEyePt.x;
				player_list[MyPlayerNum].y = m_vEyePt.y;
				player_list[MyPlayerNum].z = m_vEyePt.z;
				player_list[MyPlayerNum].rot_angle = 0;
				player_list[MyPlayerNum].bIsPlayerAlive = TRUE;
				player_list[MyPlayerNum].bStopAnimating = FALSE;
				player_list[MyPlayerNum].current_weapon = 0;
				player_list[MyPlayerNum].current_car = 0;
				player_list[MyPlayerNum].current_frame = 0;
				player_list[MyPlayerNum].current_sequence = 0;
				player_list[MyPlayerNum].bIsPlayerInWalkMode = TRUE;
				player_list[MyPlayerNum].health = 100;

				SendActionFlagMessage(MyRRnetID, TRUE, APPMSG_RESPAWN);
		
				
				KillTimer(hWnd, RESPAWN_TIMER);
			}
			break;

		case WM_INITMENUPOPUP:
			CheckMenuItem (hMenu, ID_TEXTURES_ALPHATRANSPARENCY, bEnableAlphaTransparency ? MF_CHECKED : MF_UNCHECKED);
			CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
			
			if(ShadeMode == D3DSHADE_FLAT)
			{
				CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
				CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
			}
			if(ShadeMode == D3DSHADE_GOURAUD)
			{
				CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
				CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
			}

			break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
				case MENU_ABOUT:
					Pause(TRUE);
					DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
					Pause(FALSE);
                break;

				case ID_MULTIPLAYER_DISCONNECT:
					PrintMessage(NULL, "MsgProc - ID_MULTIPLAYER_DISCONNECT", NULL, LOGFILE_ONLY);
					RrDestroyPlayer();
					break;

				case ID_MULTIPLAYER_CONNECT:

				if(multiplay_flag == TRUE)
					break;

				// See if we were launched from a lobby server
				hr = DPConnect_CheckForLobbyLaunch( &bLaunchedByLobby );
				if( FAILED(hr) )
				{
					if( hr == DPERR_USERCANCEL )
						return S_OK;
					return hr;
				}

				if( !bLaunchedByLobby )
				{
					// If not, the first step is to prompt the user about the network 
					// connection and which session they would like to join or 
					// if they want to create a new one.
					nExitCode = DPConnect_StartDirectPlayConnect( hInstApp, FALSE );

					// See the above EXITCODE #defines for what nExitCode could be 
					if( nExitCode == EXITCODE_QUIT )
					{
						// The user canceled the multiplayer connect.
						// The sample will now quit.
						return E_ABORT;
					}

					if( nExitCode == EXITCODE_ERROR || g_pDP == NULL )
					{
						MessageBox( NULL, "Multiplayer connect failed. ",
									  "You may need to reboot",	
									  MB_OK | MB_ICONERROR );
            
						return E_FAIL;
					}
				}
				break;
				//
				// Possible modes
				// --------------
				// Vertex Fog (fog type)
				//		Linear (fog mode)
				//			(Can set to Range Based)
				//			Start
				//			End
				//
				// Pixel (Table) Fog (fog type)
				//		Fog Mode - Linear
				//				Start
				//				End
				//		Fog Mode - Exponential
				//				Density (0.0 - 1.0)
				//		
                case MENU_FOG_OFF:
						m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,    FALSE );
						m_toggleFog = FALSE;
						m_tableFog = FALSE;
					break;

				case MENU_VERTEXFOG:
						// Set fog renderstates
						fStart =  13.0; 
						fEnd   =  500.0;
						m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,    TRUE );
						m_pd3dDevice->SetRenderState(
								D3DRENDERSTATE_FOGCOLOR,
								RGB_MAKE(0, 0, 80)); // Highest 8 bits aren't used.

						// Set fog parameters.
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE,  D3DFOG_NONE );
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND,   *(DWORD *)(&fEnd));
						m_tableFog = FALSE;
					break;

				case MENU_VERTEXFOG_RANGE:
						// Set fog renderstates
						fStart =  13.0;
						fEnd   =  500.0;
						m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,    TRUE );
						m_pd3dDevice->SetRenderState(
								D3DRENDERSTATE_FOGCOLOR,
								RGB_MAKE(0, 0, 80)); // Highest 8 bits aren't used.

						// Set fog parameters.
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE,  D3DFOG_NONE );
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
						m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND,   *(DWORD *)(&fEnd));

						// Enable range-based fog if desired (only supported for vertex fog).
						// For this example, it is assumed that fUseRange is set to a nonzero value
						// only if the driver exposes the D3DPRASTERCAPS_FOGRANGE capability.
						// 
						// Note: this is slightly more performance intensive
						//       than non-range-based fog.
						m_pd3dDevice->SetRenderState(
														D3DRENDERSTATE_RANGEFOGENABLE,
														TRUE);					
						m_tableFog = FALSE;
					break;

				case MENU_TABLEFOGLIN:  // Pixel fog - linear
					m_tableFog = TRUE;
					fStart =  (float)0.8; 
					fEnd   =  100.0; 
 
					// Enable fog blending.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
 
					// Set the fog color.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 80));
		
					// Set fog parameters.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE,  D3DFOG_NONE );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)(&fStart));
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGEND,   *(DWORD *)(&fEnd));
					break;

				case MENU_TABLEFOGEXP:  // Pixel fog - exponential
					m_tableFog = TRUE;
					fDensity = 0.01f;  // for exponential modes
 
					// Enable fog blending.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
 
					// Set the fog color.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 80));
		
					// Set fog parameters.
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGVERTEXMODE,  D3DFOG_NONE );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)(&fDensity));
					break;

				case MENU_TABLEFOGEXP2:  // Pixel fog - exponential
					m_tableFog = TRUE;
					fDensity = 0.01f;  // for exponential modes
 
					// Enable fog blending.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
 
					// Set the fog color.
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0, 0, 80));
		
					// Set fog parameters.
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGVERTEXMODE,  D3DFOG_NONE );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2 );
					m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)(&fDensity));
					break;

				case IDM_SELECTINPUTDEVICE:
					DialogBox( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
					           MAKEINTRESOURCE(IDD_SELECTINPUTDEVICE), hWnd,
					           (DLGPROC)InputDeviceSelectProc );
					break;

				// Shading selection
				case MENU_FLAT_SHADING:
					CheckMenuItem (hMenu, MENU_FLAT,	MF_CHECKED);
					CheckMenuItem (hMenu, MENU_GOURAUD, MF_UNCHECKED);
					ShadeMode = D3DSHADE_FLAT;
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
												D3DSHADE_FLAT );
					break;

				case MENU_GOURAUD_SHADING:
					CheckMenuItem (hMenu, MENU_FLAT,	MF_UNCHECKED);
					CheckMenuItem (hMenu, MENU_GOURAUD, MF_CHECKED);
					ShadeMode = D3DSHADE_GOURAUD;
					m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,
												D3DSHADE_GOURAUD );
					break;

           
				case MENU_LIGHT_SOURCES:
					bEnableLighting = !bEnableLighting;
					CheckMenuItem (hMenu, MENU_LIGHT_SOURCES, bEnableLighting ? MF_CHECKED : MF_UNCHECKED);
					break;

				case ID_TEXTURES_ALPHATRANSPARENCY:
					bEnableAlphaTransparency = !bEnableAlphaTransparency;
					CheckMenuItem (hMenu, ID_TEXTURES_ALPHATRANSPARENCY, bEnableAlphaTransparency ? MF_CHECKED : MF_UNCHECKED);
					break;

                case IDM_EXIT:
                    PrintMessage(NULL, "MsgProc - IDM_EXIT", NULL, LOGFILE_ONLY);
					RrDestroyPlayer();
					Cleanup3DEnvironment();
                    SendMessage( hWnd, WM_CLOSE, 0, 0 );
					DestroyWindow( hWnd );
					PostQuitMessage(0);
                    exit(0);
				default:
					return CD3DApplication::MsgProc( hWnd, uMsg, wParam,
													 lParam );

            }
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

        case WM_CLOSE:
			PrintMessage(NULL, "MsgProc - WM_CLOSE", NULL, LOGFILE_ONLY);
            RrDestroyPlayer();
			Cleanup3DEnvironment();
            DestroyWindow( hWnd );
            PostQuitMessage(0);
            return 0;

		default:
			return CD3DApplication::MsgProc( hWnd, uMsg, wParam,
											 lParam );
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
  
  

void CMyD3DApplication::AddPlayerLightSource(int player_num, float x, float y, float z)
{ 
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
    D3DLIGHT7 light;


	if(bEnableLighting == FALSE)
		return;

    ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
    light.dcvDiffuse.r  = 0.0f;
    light.dcvDiffuse.g  = 0.0f;
    light.dcvDiffuse.b  = 0.0f;

	weapon_flash_r -= 0.05f; 
	weapon_flash_g = weapon_flash_r / 2.0f;

	light.dcvAmbient.r  = weapon_flash_r; //0.4f;
	light.dcvAmbient.g  = weapon_flash_g; //0.2f;
	light.dcvAmbient.b  = 0.0f;

	if(weapon_flash_r < 0)
	{
		weapon_flash_r = 0.4f;
		weapon_flash_r = 0.2f;
		player_list[player_num].bIsFiring = FALSE;
	}
	light.dcvSpecular.r  = 1.0f;
	light.dcvSpecular.g  = 1.0f;
	light.dcvSpecular.b  = 1.0f;
	
    light.dvRange       = 200.0f;

	pos_x = x;
	pos_y = -100;
	pos_z = z;

	dir_x = 0;
	dir_y = 100;
	dir_z = 0;


	light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
    light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
    light.dvFalloff       = 100.0f;

	// angle dvPhi   must be within the range 0 to pi	 radians
	// angle dvTheta must be within the range 0 to dvPhi radians

    light.dvTheta         = 1.4f;	// spotlight's inner cone
    light.dvPhi           = 2.1f;	// spotlight's outer cone
    light.dvAttenuation0  = 1.0f;
	light.dltType		  = D3DLIGHT_SPOT;
		
	// Set the light
	
	m_pd3dDevice->SetLight( num_light_sources, &light );
	m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
	num_light_sources++;
}


void CMyD3DApplication::PlayerToD3DVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_poly;
	int i_count;
	short v_index;
	float x,y,z;
	float rx,ry,rz;
	float tx,ty;
	vert_ptr tp;	
	DWORD r,g,b;
	D3DPRIMITIVETYPE p_command;
	BOOL error = TRUE;
	

	if(angle >= 360)
		angle = angle - 360;
	if(angle < 0)
		angle += 360;

	if(pmdata[pmodel_id].use_indexed_primitive == TRUE)
	{
		PlayerToD3DIndexedVertList(pmodel_id, curr_frame, angle, texture_alias, tex_flag);
		return;
	}

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	if(curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	i_count = 0;

	num_poly = pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		p_command	   = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_vert[i];
		
		for(j = 0; j < num_verts_per_poly; j++)
		{
			v_index = pmdata[pmodel_id].f[i_count];

			tp = &pmdata[pmodel_id].w[curr_frame][v_index];
			x = tp->x + x_off;
			z = tp->y + y_off;
			y = tp->z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx; 
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky; 

			r=0;
			g=0;
			b=0;

			src_v[cnt].x = D3DVAL(rx);
			src_v[cnt].y = D3DVAL(ry);
			src_v[cnt].z = D3DVAL(rz);
			src_v[cnt].tu = D3DVAL(tx);
			src_v[cnt].tv = D3DVAL(ty);

			cnt++;
			i_count++;

		}
	
		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		dp_commands[number_of_polys_per_frame] = p_command;

		if((p_command == D3DPT_TRIANGLESTRIP) || (p_command == D3DPT_TRIANGLEFAN))
			num_triangles_in_scene += ( num_verts_per_poly - 2 );

		if(p_command == D3DPT_TRIANGLELIST)
			num_triangles_in_scene += ( num_verts_per_poly / 3 );


		num_verts_in_scene +=	num_verts_per_poly;
		num_dp_commands_in_scene ++;


		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = pmdata[pmodel_id].texture_list[i]; 
						
		number_of_polys_per_frame++;

	} // end for vert_cnt
		
	return;
}

void CMyD3DApplication::PlayerToD3DIndexedVertList(int pmodel_id, int curr_frame, int angle, int texture_alias, int tex_flag)
{ 
	int i,j;
	int num_verts_per_poly;
	int num_faces_per_poly;
	int num_poly;
	int poly_command;
	int i_count, face_i_count;
	float x,y,z;
	float rx,ry,rz;
	float tx,ty;
	FILE *fp;

	if(curr_frame >= pmdata[pmodel_id].num_frames)
		curr_frame = 0;

	curr_frame = 0;
	cosine = cos_table[angle];
	sine   = sin_table[angle];

	i_count = 0;
	face_i_count = 0;

	if(rendering_first_frame == TRUE)
		fp = fopen("rrlogfile.txt","a");

	// process and transfer the model data from the pmdata structure 
	// to the array of D3DVERTEX structures, src_v 
	
	num_poly = pmdata[pmodel_id].num_polys_per_frame;
	
	for(i = 0; i < num_poly; i++)
	{
		poly_command	   = pmdata[pmodel_id].poly_cmd[i];
		num_verts_per_poly = pmdata[pmodel_id].num_verts_per_object[i];
		num_faces_per_poly = pmdata[pmodel_id].num_faces_per_object[i];
		
		for(j = 0; j < num_verts_per_poly; j++)
		{
			
			x = pmdata[pmodel_id].w[curr_frame][i_count].x + x_off;
			z = pmdata[pmodel_id].w[curr_frame][i_count].y + y_off;
			y = pmdata[pmodel_id].w[curr_frame][i_count].z + z_off;

			rx = wx + (x*cosine - z*sine); 
			ry = wy + y;
			rz = wz + (x*sine + z*cosine);
				
			tx = pmdata[pmodel_id].t[i_count].x * pmdata[pmodel_id].skx; 
			ty = pmdata[pmodel_id].t[i_count].y * pmdata[pmodel_id].sky; 

			src_v[cnt].x =  D3DVAL(rx);
			src_v[cnt].y =  D3DVAL(ry);
			src_v[cnt].z =  D3DVAL(rz);
			src_v[cnt].tu = D3DVAL(tx);
			src_v[cnt].tv = D3DVAL(ty);

			if(rendering_first_frame == TRUE)
			{
				fprintf( fp, "%f %f %f, ", 
				//cnt,
				src_v[cnt].x,
				src_v[cnt].y,
				src_v[cnt].z);
			}

			cnt++;
			i_count++;

		} // end for j
	
		for(j = 0; j < num_faces_per_poly*3; j++)
		{
			src_f[cnt_f] = pmdata[pmodel_id].f[face_i_count];

			if(rendering_first_frame == TRUE)
			{
				//fprintf( fp, "%d ", src_f[cnt_f] );
			}

			cnt_f++;
			face_i_count++;
		}

		verts_per_poly[number_of_polys_per_frame] = num_verts_per_poly;
		faces_per_poly[number_of_polys_per_frame] = num_faces_per_poly;

		dp_command_index_mode[number_of_polys_per_frame] = USE_INDEXED_DP;
		dp_commands			 [number_of_polys_per_frame] = D3DPT_TRIANGLELIST;
		
		num_triangles_in_scene += num_faces_per_poly;
		num_verts_in_scene	   += num_verts_per_poly;
		num_dp_commands_in_scene ++;

		if (tex_flag == USE_PLAYERS_SKIN)
			texture_list_buffer[number_of_polys_per_frame] = texture_alias;
		else
			texture_list_buffer[number_of_polys_per_frame] = pmdata[pmodel_id].texture_list[i]; 
								
		number_of_polys_per_frame++;

	} // end for vert_cnt	
	
	if(rendering_first_frame == TRUE)
	{
	fprintf( fp, " \n\n"); 
	fclose(fp);
	}

	return;
}


void CMyD3DApplication::ObjectToD3DVertList(int ob_type, int angle, int oblist_index)
{ 
	int ob_vert_count = 0;
	int poly;
	int num_vert;
	int vert_cnt;
	int w, i;
	float x,y,z;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	D3DPRIMITIVETYPE poly_command; 
	BOOL poly_command_error = TRUE;
 	
	
	if( (oblist[oblist_index].light_source->command != 0 ) &&
		(oblist_overlite_flags[oblist_index] == FALSE)     &&
		(bEnableLighting) )
						
	{

		oblist_overlite_flags[oblist_index] = TRUE;

		// Set up the light structure
		D3DLIGHT7 light;
		ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
		light.dcvDiffuse.r  = 0.0f;
		light.dcvDiffuse.g  = 0.0f;
		light.dcvDiffuse.b  = 0.0f;

		light.dcvAmbient.r  = 0.4f;
		light.dcvAmbient.g  = 0.2f;
		light.dcvAmbient.b  = 0.0f;

		light.dcvSpecular.r  = 1.0f;
		light.dcvSpecular.g  = 1.0f;
		light.dcvSpecular.b  = 1.0f;
	 
		//light.dvRange     = D3DLIGHT_RANGE_MAX;
		light.dvRange       = 200.0f;

		pos_x = oblist[oblist_index].light_source->position_x;
		pos_y = oblist[oblist_index].light_source->position_y;
		pos_z = oblist[oblist_index].light_source->position_z;

		dir_x = oblist[oblist_index].light_source->direction_x;
		dir_y = oblist[oblist_index].light_source->direction_y;
		dir_z = oblist[oblist_index].light_source->direction_z;


		switch( oblist[oblist_index].light_source->command )
		{
		
			case POINT_LIGHT_SOURCE:
				light.dvPosition      = D3DVECTOR( pos_x, pos_y, pos_z ); 
				light.dvAttenuation1  = 1.0f;
				light.dltType		  = D3DLIGHT_POINT; 
				break;

			case DIRECTIONAL_LIGHT_SOURCE:
				light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
				light.dltType		  = D3DLIGHT_DIRECTIONAL; 
				break;

			case SPOT_LIGHT_SOURCE:
				light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
				light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
				light.dvFalloff       = 100.0f;

				// angle dvPhi   must be within the range 0 to pi	 radians
				// angle dvTheta must be within the range 0 to dvPhi radians

				light.dvTheta         = 1.4f;	// spotlight's inner cone
				light.dvPhi           = 2.1f;	// spotlight's outer cone
				light.dvAttenuation0  = 1.0f;
				light.dltType		  = D3DLIGHT_SPOT;
				break;
		}

		// Set the light
	
		m_pd3dDevice->SetLight( num_light_sources, &light );
		m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
		num_light_sources++;
	}

	if(bSkipThisCell == TRUE)
		return;

	cosine = cos_table[angle];
	sine   = sin_table[angle];

	ob_vert_count = 0;
	poly = num_polys_per_object[ob_type];

	for(w=0; w<poly; w++)
	{
		num_vert = obdata[ob_type].num_vert[w];

		for(vert_cnt = 0; vert_cnt < num_vert; vert_cnt++)
		{
			x = obdata[ob_type].v[ob_vert_count].x;
			y = obdata[ob_type].v[ob_vert_count].y;
			z = obdata[ob_type].v[ob_vert_count].z;

			tx[vert_cnt] = obdata[ob_type].t[ob_vert_count].x; 
			ty[vert_cnt] = obdata[ob_type].t[ob_vert_count].y; 

			mx[vert_cnt] = wx + (x*cosine - z*sine); 
			my[vert_cnt] = wy + y;
			mz[vert_cnt] = wz + (x*sine + z*cosine);

			ob_vert_count++;

		} // end for vert_cnt
		
		verts_per_poly[number_of_polys_per_frame] = num_vert;
		poly_command = obdata[ob_type].poly_cmd[w];

		ctext = obdata[ob_type].tex[w];
		texture_list_buffer[number_of_polys_per_frame] = ctext;


		if(obdata[ob_type].use_texmap[w] == FALSE) 
		{
			for(i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++)
			{
			
				src_v[cnt].x  = D3DVAL(mx[i]);
				src_v[cnt].y  = D3DVAL(my[i]);
				src_v[cnt].z  = D3DVAL(mz[i]);
				src_v[cnt].tu = D3DVAL(tx[i]); 
				src_v[cnt].tv = D3DVAL(ty[i]);
				cnt++;
			}
			
		}	
		else
		{
			for(i = 0; i < verts_per_poly[number_of_polys_per_frame]; i++)
			{
				src_v[cnt].x  = D3DVAL(mx[i]);
				src_v[cnt].y  = D3DVAL(my[i]);
				src_v[cnt].z  = D3DVAL(mz[i]);
				src_v[cnt].tu = D3DVAL(TexMap[ctext].tu[i]); 
				src_v[cnt].tv = D3DVAL(TexMap[ctext].tv[i]);
				cnt++;
			}
		}

		dp_commands[number_of_polys_per_frame] = poly_command;
		dp_command_index_mode[number_of_polys_per_frame] = USE_NON_INDEXED_DP;
		number_of_polys_per_frame++;
		
		if((poly_command == D3DPT_TRIANGLESTRIP) || (poly_command == D3DPT_TRIANGLEFAN))
			num_triangles_in_scene += ( num_vert - 2 );

		if(poly_command == D3DPT_TRIANGLELIST)
			num_triangles_in_scene += ( num_vert / 3 );

		num_verts_in_scene +=	num_vert;
		num_dp_commands_in_scene ++;

		if((poly_command < 0) || (poly_command > 6))
			PrintMessage(NULL, "CMyD3DApplication::ObjectToD3DVertList -  ERROR UNRECOGNISED COMMAND", NULL , LOGFILE_ONLY);
	
	} // end for w

	return;
}


//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render3DEnvironment()
{
    HRESULT hr;

    // Check the cooperative level before rendering
    if( FAILED( hr = m_pDD->TestCooperativeLevel() ) )
    {
        switch( hr )
        {
            case DDERR_EXCLUSIVEMODEALREADYSET:
            case DDERR_NOEXCLUSIVEMODE:
                // Do nothing because some other app has exclusive mode
                return S_OK;

            case DDERR_WRONGMODE:
                // The display mode changed on us. Resize accordingly
                if( m_pDeviceInfo->bWindowed )
                    return Change3DEnvironment();
                break;
        }
        return hr;
    }

    // Get the relative time, in seconds
    FLOAT fTime = ( timeGetTime() - GetBaseTime() ) * 0.001f;

	UpdateControls();

    // FrameMove (animate) the scene
    if( GetbFrameMoving() || GetSingleStep() )
    {
        if( FAILED( hr = FrameMove( fTime ) ) )
            return hr;

        SetSingleStep(FALSE);
    }

    // If the display is in a stereo mode, render the scene from the left eye
    // first, then the right eye.
    if( m_bAppUseStereo && m_pDeviceInfo->bStereo && !m_pDeviceInfo->bWindowed )
    {
        // Render the scene from the left eye
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matLeftView );
        if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTargetLeft, 0 ) ) )
            return hr;
        if( FAILED( hr = Render() ) )
            return hr;

        //Render the scene from the right eye
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matRightView );
        if( FAILED( hr = m_pd3dDevice->SetRenderTarget( m_pddsRenderTarget, 0 ) ) )
            return hr;
        if( FAILED( hr = Render() ) )
            return hr;
    } 
    else 
    {
        // Set center viewing matrix if app is stereo-enabled
        if( m_bAppUseStereo )
            m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matView );

        // Render the scene as normal
        if( FAILED( hr = Render() ) )
            return hr;
    }

    // Show the frame rate, etc.
    if( m_bShowStats )
        ShowStats();

    // Show the frame on the primary surface.
    if( FAILED( hr = (GetFramework())->ShowFrame() ) )
    {
        if( DDERR_SURFACELOST != hr )
            return hr;

        (GetFramework())->RestoreSurfaces();
        RestoreSurfaces();
    }

    return S_OK;
}


void CMyD3DApplication::InitRRvariables()
{
	int i;
	float fangle;
	float dx, dy;
	RECT szClient;

	GetClientRect(m_hWnd, &szClient);

	dx = (float)(szClient.right - szClient.left);
	dy = (float)(szClient.bottom - szClient.top);
	ASPECT_RATIO = 1.0f;

	PrintMessage(NULL, "CMyD3DApplication::InitRRvariables - starting", NULL, LOGFILE_ONLY);

	walk_mode_enabled = TRUE;
	current_gun = 0;

	car_speed = (float)0;
	angy = 0; 
	look_up_ang = 0;
			
	oblist = new OBJECTLIST[10000]; 
	obdata = new OBJECTDATA[100]; 

	src_v = new D3DVERTEX[MAX_NUM_VERTICES]; 

	player_list				= new PLAYER[10];
	car_list				= new PLAYER[10];
	debug					= new PLAYER[10];
	your_gun				= new GUNLIST[10];
	other_players_guns		= new GUNLIST[10];
	pmdata					= new PLAYERMODELDATA[10];

	poly_clip_flags			= new int[MAX_NUM_QUADS]; 
	poly_clip				= new int[MAX_NUM_QUADS];
	texture_list_buffer		= new int[MAX_NUM_QUADS];

	dp_command_index_mode	= new BOOL[MAX_NUM_QUADS];   
	dp_commands				= new D3DPRIMITIVETYPE[MAX_NUM_QUADS]; 

	verts_per_poly			= new int[MAX_NUM_QUADS]; 
	faces_per_poly			= new int[MAX_NUM_QUADS];
	src_f					= new int[MAX_NUM_FACE_INDICES];

	oblist_overdraw_flags   = new BOOL[MAX_NUM_QUADS];
	oblist_overlite_flags   = new BOOL[MAX_NUM_QUADS];
	num_vert_per_object		= new int[100];

	cell_length = new int*[200];

	for(i=0;i<200;i++)
		cell_length[i] = new int[200];


	draw_flags = new BOOL*[200];

	for(i=0;i<200;i++)
		draw_flags[i] = new BOOL[200];

	for(i=0;i<256;i++)
		DelayKey2[i] = FALSE;

	for(i = 0; i < 10; i++)
	{
		player_list[i].frags = 0;	
		player_list[i].x = 500;
		player_list[i].y = 22;
		player_list[i].z = 500;
		player_list[i].rot_angle = 0;
		player_list[i].model_id = 0;
		player_list[i].skin_tex_id = 0;
		player_list[i].bIsFiring = FALSE;
		player_list[i].ping = 0;
		player_list[i].health = 100;
		player_list[i].bIsPlayerAlive = TRUE;
		player_list[i].bStopAnimating = FALSE;
		player_list[i].current_weapon = 0;
		player_list[i].current_car = 0;
		player_list[i].current_frame = 0;
		player_list[i].current_sequence = 0;
		player_list[i].bIsPlayerInWalkMode = TRUE;
		player_list[i].RRnetID = g_LocalPlayerDPID;
		player_list[i].bIsPlayerValid = FALSE;
		strcpy(player_list[i].name, "");
	}

	num_players = 1;

	MyPing = 0;
	MyHealth = 100;

	for(i=0;i<256;i++) 
		DelayKey2[i] = FALSE;

	for(i=0;i<=360;i++) 
	{
		fangle = (float)i * k;
		sin_table[i] = (float)sin(fangle);
		cos_table[i] = (float)cos(fangle);
	}

	m_vEyePt.x = 700;
	m_vEyePt.y = 22;
	m_vEyePt.z = 700;


	your_gun[0].sound_id = SND_FIRE_AR15;
	your_gun[1].sound_id = SND_FIRE_MP5;
	current_gun = 0;
	
	PrintMemAllocated(sizeof(OBJECTLIST[10000]), "oblist"); 
	PrintMemAllocated(sizeof(OBJECTDATA[100]), "obdata"); 

	PrintMemAllocated(sizeof(D3DVERTEX[MAX_NUM_VERTICES]), "src_v"); 

	PrintMemAllocated(sizeof(PLAYER[10]), "player_list"); 
	PrintMemAllocated(sizeof(PLAYER[10]), "car_list"); 
	PrintMemAllocated(sizeof(PLAYER[10]), "debug"); 
	PrintMemAllocated(sizeof(GUNLIST[10]), "your_gun"); 
	PrintMemAllocated(sizeof(GUNLIST[10]), "other_players_guns"); 
	PrintMemAllocated(sizeof(PLAYERMODELDATA[10]),"pmdata"); 

	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip_flags"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "poly_clip"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "texture_list_buffer"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_command_index_mode"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "dp_commands"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "verts_per_poly"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_QUADS]), "faces_per_poly"); 
	PrintMemAllocated(sizeof(int[MAX_NUM_FACE_INDICES]), "src_f"); 
	
	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overdraw_flags");
	PrintMemAllocated(sizeof(BOOL[MAX_NUM_QUADS]), "oblist_overlite_flags");

	PrintMemAllocated(total_allocated_memory_count, "TOTAL"); 

}


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

BOOL CMyD3DApplication::LoadRR_Resources()
{
	if (!pCWorld->LoadObjectData(m_hWnd,"objects.dat"))
	{
		PrintMessage(m_hWnd, "LoadObjectData failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}
	
	if (!pCWorld->LoadWorldMap  (m_hWnd,"level1.map"))
	{
		PrintMessage(m_hWnd, "LoadWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	if (!pCWorld->InitPreCompiledWorldMap(m_hWnd,"level1.cmp"))
	{
		PrintMessage(m_hWnd, "InitPreCompiledWorldMap failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}	
	
	if (!pCWorld->LoadImportedModelList(m_hWnd,"modellist.dat"))
	{
		PrintMessage(m_hWnd, "LoadImportedModelList failed", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	OpenSoundFile("colt.wav", 0);
	OpenSoundFile("engine.wav", 1);
	
	AddPlayer(650, 13, 1000, 0);

	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	
	CMyD3DApplication d3dApp;
	HRESULT hr;

	d3dApp.hInstApp = hInst;

    if( FAILED( d3dApp.Create( hInst, strCmdLine ) ) )
        return 0;

    // Init DirectSound
    if( FAILED( d3dApp.InitDirectSound( d3dApp.Get_hWnd() ) ) )
    {
        MessageBox( NULL, "Error initializing DirectSound.  Sample will now exit.", 
                            "DirectSound Sample", MB_OK | MB_ICONERROR );
        return FALSE;
    }

    pCMyApp->m_bBufferPaused = FALSE;

	for (int ii = 0; ii < MAX_SOUND_BUFFERS; ii++)
		g_pDSBuffer[ii] = NULL;

	//
	// Create the DInput object
    //
	if( FAILED(d3dApp.CreateDInput( d3dApp.Get_hWnd() ) ) )
    {
        return FALSE;
    }

    // Read information from registry
	ReadDpInfoFromRegistry();

    g_hDPMessageEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

    if( FAILED( hr = CoInitialize( NULL ) ) )
        return FALSE;

    // Create a keyboard device
    if( FAILED(d3dApp.CreateInputDevice( d3dApp.Get_hWnd(), g_Keyboard_pDI, 
								g_Keyboard_pdidDevice2, 
								GUID_SysKeyboard, &c_dfDIKeyboard,
                                DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) ) )
    {
        return FALSE;
    }

	d3dApp.InitRRvariables();
//	d3dApp.LoadCrossHair();
	//
	// Textures need to have been loaded by now
	//
	d3dApp.LoadRR_Resources();

    return d3dApp.Run();

	PrintMessage(NULL, "Quitting", NULL, LOGFILE_ONLY);
		
	d3dApp.DestroyInputDevice();
    d3dApp.DestroyDInput();

	if( SUCCEEDED( hr ) )
    {
        // Write information to the registry
        WriteRegisteryInfo();
    }

    // Cleanup DirectPlay
    if( g_pDP )
    {
		PrintMessage(NULL, "Cleanup DirectPlay FAILED", NULL, LOGFILE_ONLY);
		
        g_pDP->DestroyPlayer( g_LocalPlayerDPID );
        g_pDP->Close();

        SAFE_DELETE_ARRAY( g_pDPLConnection );
        SAFE_RELEASE( g_pDPLobby );
        SAFE_RELEASE( g_pDP );
    }
	
	//	PrintMessage(NULL, "Cleanup DirectPlay FAILED", NULL, LOGFILE_ONLY);

    d3dApp.FreeDirectSound();
	
	CoUninitialize();

    CloseHandle( g_hDPlaySampleRegKey );
    CloseHandle( g_hDPMessageEvent );
}





//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    //WORD i, j, ind, v;

    m_pBackground[0] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 0.0f, 0.6f );
    m_pBackground[1] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 0.0f, 0.0f );
    m_pBackground[2] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 1.0f, 0.6f );
    m_pBackground[3] = D3DTLVERTEX( D3DVECTOR( 0, 0, 0.99f ), 0.5, -1, 0, 1.0f, 0.0f );

    D3DTextr_CreateTextureFromFile( "..\\textures\\Cloud3.bmp" );


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove( FLOAT fTimeKey )
{
    D3DMATRIX matView;
	float r = 100.0f; 
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;
	float lx, ly, lz;
	float radius = 50.0f; // used for flashlight
	float gun_angle;
	int i;

    // Move the clouds
    FLOAT t = fTimeKey/40.0f;
    FLOAT u = (((DWORD)(t*10000))%10000)/10000.0f;
    m_pBackground[0].tu = 0 - u;
    m_pBackground[1].tu = 0 - u;
    m_pBackground[2].tu = 1 - u;
    m_pBackground[3].tu = 1 - u;

	// check that angy is within limits for sin and cos table

	if(angy >= 360)
		angy = angy - 360;

	if(angy < 0)
		angy += 360;

	gun_angle = -angy + (float)90;
	
	if(gun_angle >=360)
		gun_angle = gun_angle -360;
	if(gun_angle <0)
		gun_angle = gun_angle +360;
	
	i = current_gun;
	your_gun[i].rot_angle = gun_angle;
	your_gun[i].x = m_vEyePt.x;
	your_gun[i].y = (float)9+ m_vEyePt.y - 22;
	your_gun[i].z = m_vEyePt.z;


	for(i = 0; i < num_light_sources_in_map; i++)
		m_pd3dDevice->LightEnable( (DWORD)i, FALSE );

	num_light_sources = 0;

	// Set up the light structure
    D3DLIGHT7 light;
    ZeroMemory( &light, sizeof(D3DLIGHT7) );
    
    light.dcvDiffuse.r  = 0.0f;
    light.dcvDiffuse.g  = 0.0f;
    light.dcvDiffuse.b  = 0.0f;

	light.dcvAmbient.r  = 0.3f;
	light.dcvAmbient.g  = 0.3f;
	light.dcvAmbient.b  = 0.3f;

	light.dcvSpecular.r  = 0.0f;
	light.dcvSpecular.g  = 0.0f;
	light.dcvSpecular.b  = 0.0f;
	 
    light.dvRange       = 500.0f; // D3DLIGHT_RANGE_MAX

	// Calculate the flashlight's lookat point, from 
	// the player's view direction angy.

	lx = m_vEyePt.x + radius * sinf(angy * k); 
	ly = 0;
	lz = m_vEyePt.z + radius * cosf(angy * k);

	// Calculate direction vector for flashlight
	dir_x = lx - m_vEyePt.x;
	dir_y = 0; //ly - m_vEyePt.y;
	dir_z = lz - m_vEyePt.z;

	// set flashlight's position to player's position
	pos_x = m_vEyePt.x;
	pos_y = 10; //m_vEyePt.y;
	pos_z = m_vEyePt.z;

	light.dvPosition	  = D3DVECTOR( pos_x, pos_y, pos_z );
    light.dvDirection     = D3DVECTOR( dir_x, dir_y, dir_z );
    light.dvFalloff       = 1.0f;
    light.dvTheta         = 0.1f;	// spotlight's inner cone
    light.dvPhi           = 0.3f;	// spotlight's outer cone
    light.dvAttenuation0  = 0.3f;
	light.dltType		  = D3DLIGHT_SPOT;
		
    // Set the light
	
	if(bIsFlashlightOn == TRUE)
	{
		m_pd3dDevice->SetLight( num_light_sources, &light );
		m_pd3dDevice->LightEnable( (DWORD)num_light_sources, TRUE );
		num_light_sources++;
	}
	

	// Calculate the players lookat point, from 
	// the player's view direction angy.

	m_vLookatPt.x = m_vEyePt.x + r * sinf(angy * k); 
	m_vLookatPt.y = m_vEyePt.y + r * sinf(look_up_ang * k);
	m_vLookatPt.z = m_vEyePt.z + r * cosf(angy * k);

	// set World, View, and Projection matrices
	D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetViewMatrix( matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	D3DUtil_SetProjectionMatrix( matProj, FOV, ASPECT_RATIO, Z_NEAR, Z_FAR );

    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );


    return S_OK;
}


BOOL CMyD3DApplication::RrBltCrossHair(LPDIRECTDRAWSURFACE7 lpddsTexturePtr)
{
	RECT szClient;
	RECT rc_crosshair;

	rc_crosshair.left = 0;
	rc_crosshair.top = 0;
	rc_crosshair.right = 30;
	rc_crosshair.bottom = 30;

	if(GetFramework()->GetBackBuffer() == NULL)
		return FALSE;

	GetClientRect(m_hWnd, &szClient);
	GetFramework()->GetBackBuffer()->BltFast((GetFramework()->GetRenderWidth()  / 2) - 15,
								  (GetFramework()->GetRenderHeight() / 2) - 15, 
							      lpddsTexturePtr, 
								  &rc_crosshair,
							      DDBLTFAST_SRCCOLORKEY);
	return TRUE;
}

void CMyD3DApplication::AddPlayer(float x, float y, float z, float rot_angle)
{
	player_list[num_players].bIsPlayerValid = TRUE;

	player_list[num_players].x = x;
	player_list[num_players].y = y;
	player_list[num_players].z = z;
	player_list[num_players].rot_angle = rot_angle;

	player_list[num_players].skin_tex_id = 27;
	num_players++;

}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	int i, j, t;
	int angle;
	int last_texture_number;
	int oblist_index;
	int ob_type;
	int vert_index;
	int face_index;	
	int cell_x,cell_z;
	int curr_wep;
	int curr_car;
	int f_index;
	int texture_alias_number;
	int vi,q,ix,iz,icnt;
	int vol_length;
	int vol_x[MAX_NUM_QUADS];
	int vol_z[MAX_NUM_QUADS];

	DWORD dwIndexCount;  
	DWORD dwVertexCount; 
	DWORD color_key=0;
	BOOL use_player_skins_flag = TRUE;
	LPDIRECTDRAWSURFACE7 lpDDsurface;

	float skx, sky;
	D3DPRIMITIVETYPE command;
	D3DVERTEX temp_v[2000]; // debug
	HRESULT hr;
	int ap_cnt = 0;
	int	alpha_poly_index[MAX_NUM_QUADS];
	int	alpha_vert_index[MAX_NUM_QUADS];
	int	alpha_face_index[MAX_NUM_QUADS];
	

	IsRenderingOk = TRUE;

	if(rendering_first_frame == TRUE)
	{
		RRAppActive = TRUE;
		PrintMessage(m_hWnd, "RenderScene : rendering first frame", NULL, LOGFILE_ONLY);
	}


	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		               0x00000000, 1.0f, 0L );	

    D3DCLIPSTATUS status={D3DCLIPSTATUS_EXTENTS2, 0, 2048.0f, 0.0f, 2048.0f, 0.0f, 0.0f, 0.0f};
	
	number_of_polys_per_frame = 0;
	num_triangles_in_scene = 0 ;
	num_verts_in_scene = 0;
	num_dp_commands_in_scene = 0 ;

	poly_cnt	= 0;
	cnt			= 0;
	icnt		= 0;  
	face_index  = 0; 
	cnt_f		= 0;  
	
	cell_x = (int)(m_vEyePt.x / 256);
	cell_z = (int)(m_vEyePt.z / 256);

	player_x = m_vEyePt.x;
	player_z = m_vEyePt.z;


	for(i = 0; i < MAX_NUM_QUADS; i++)
	{
		oblist_overdraw_flags[i] = FALSE;
		oblist_overlite_flags[i] = FALSE;
	}

	if(angy>=360)
		angy=angy-360;

	if(angy<0)
		angy+=360;


	// Work out which direction player is facing in
	// i.e north, east, south, or west
	// and store the numbers of the cells which are in view.	
	// Roadrage works out what cells are visible by simply
	// looking at a rectangular 5 x 6 block of cells directly 
	// infront of the player. 

	if((angy >= 315) || (angy <= 45)) // player facing North
	{
		for(ix=-2;ix<=2;ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z-1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z+1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z+2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z+3;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5] = cell_z+4;

			icnt+=6;
		}
	}

	if( (angy > 225) && (angy < 315) ) // player facing West
	{
		for(iz=-2;iz<=2;iz++)
		{
			vol_x[icnt]   = cell_x +1,  vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x ,	vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x - 1, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x - 2, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x - 3, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x - 4, vol_z[icnt+5] = cell_z + iz;

			icnt+=6;
		}
	}

	if( (angy >= 135) && (angy <= 225)) // player facing South
	{
		for(ix=-2;ix<=2;ix++)
		{
			vol_x[icnt]   = cell_x + ix, vol_z[icnt]   = cell_z+1;
			vol_x[icnt+1] = cell_x + ix, vol_z[icnt+1] = cell_z;
			vol_x[icnt+2] = cell_x + ix, vol_z[icnt+2] = cell_z-1;
			vol_x[icnt+3] = cell_x + ix, vol_z[icnt+3] = cell_z-2;
			vol_x[icnt+4] = cell_x + ix, vol_z[icnt+4] = cell_z-3;
			vol_x[icnt+5] = cell_x + ix, vol_z[icnt+5] = cell_z-4;

			icnt+=6;
		}
	}

	if((angy > 45) && (angy < 135)) // player facing East
	{
		for(iz=-2;iz<=2;iz++)
		{
			vol_x[icnt]   = cell_x-1,	vol_z[icnt]   = cell_z + iz;
			vol_x[icnt+1] = cell_x,		vol_z[icnt+1] = cell_z + iz;
			vol_x[icnt+2] = cell_x + 1, vol_z[icnt+2] = cell_z + iz;
			vol_x[icnt+3] = cell_x + 2, vol_z[icnt+3] = cell_z + iz;
			vol_x[icnt+4] = cell_x + 3, vol_z[icnt+4] = cell_z + iz;
			vol_x[icnt+5] = cell_x + 4, vol_z[icnt+5] = cell_z + iz;

			icnt+=6;
		}
	}

	number_of_polys_per_frame = 0;
	cnt = 0;

	vol_length = icnt;

	
	for(vi=0;vi<vol_length;vi++)
	{		
		cell_x = vol_x[vi];
		cell_z = vol_z[vi];
	
		if( (vi == 0)  || (vi == 6)  || (vi == 12) || 
			(vi == 18) || (vi == 24) || (vi == 30) )
		{
			bSkipThisCell = TRUE;
		}
		else
			bSkipThisCell = FALSE;

		// check that this cell is within the map limits
		if( ((cell_x >=  0) && (cell_z >=  0)) &&  
			((cell_x < 200) && (cell_z < 200)) )
		{
			// check if there is a list of map objects allocated to this cell
			if(cell[cell_x][cell_z] != NULL) 
			{					
				// get the number of objects which need to be drawn 
				// for this cell
				clength = cell_length[cell_x][cell_z];
				
				for(q=0;q<clength;q++)
				{	
					// get the object's map ID
					oblist_index = cell[cell_x][cell_z][q];

					if( (oblist_index >= 0) && (oblist_index < oblist_length) )
					{
						// check map object list and prevent the same 
						// objects being drawn more than once

						if(oblist_overdraw_flags[oblist_index] == FALSE)
						{
							if(bSkipThisCell == FALSE)
								oblist_overdraw_flags[oblist_index] = TRUE;

							wx = oblist[oblist_index].x;
							wy = oblist[oblist_index].y;
							wz = oblist[oblist_index].z;
		
							angle = (int)oblist[oblist_index].rot_angle;
							ob_type = oblist[oblist_index].type;

							ObjectToD3DVertList(ob_type, angle, oblist_index);

						}
					}
				} // end for q
			} // end if
		} // end if
	} // end for vi


	// Todo :  Load D3D Vertex array here for characters and guns etc 

	// DRAW YOUR GUN ///////////////////////////////////////////
	
	if(bInWalkMode )
	{
		skx = (float)0.40000000 / (float)256;
		sky = (float)1.28000000 / (float)256;
		use_player_skins_flag = 1;

		i = current_gun;

		if(current_gun < num_your_guns)
		{
			wx = your_gun[i].x; 
			wy = your_gun[i].y;
			wz = your_gun[i].z; 

			x_off = your_gun[i].x_offset;
			y_off = your_gun[i].y_offset;
			z_off = your_gun[i].z_offset; 

			ob_type = your_gun[i].model_id;
			current_frame = your_gun[i].current_frame;
			angle = (int)your_gun[i].rot_angle;
		
			PlayerToD3DVertList(ob_type, 
								current_frame, 
								angle, 
								NULL, 
								USE_DEFAULT_MODEL_TEX); 
		}
	}



	// DRAW Players ///////////////////////////////////////////
		
	ob_type = 30;
	angle = 0;	

	x_off = 0;
	y_off = 0;
	z_off = 0;

	for(i = 0; i < num_players; i++)
	{
		if(player_list[i].bIsPlayerValid == TRUE)
		{
			if((multiplay_flag == TRUE) && (player_list[i].RRnetID == MyRRnetID))
			{
				if(player_list[i].bIsFiring == TRUE)
				{
					wx = player_list[i].x; 
					wy = player_list[i].y;		
					wz = player_list[i].z;
					AddPlayerLightSource(i, wx, wy, wz);
				}
				i++;	
			}
		
			wx = player_list[i].x; 
			wy = player_list[i].y;		
			wz = player_list[i].z;

		
			use_player_skins_flag = 1;
			current_frame = player_list[i].current_frame;
	
			if(player_list[i].bIsPlayerInWalkMode == TRUE)
			{
				angle = 360 - (int)player_list[i].rot_angle + 90;

				if( (abs((int)(wx - player_x)) < 512) && (abs((int)(wz - player_z) < 512)) ) 
				{
					PlayerToD3DVertList(player_list[i].model_id, 
								current_frame,	angle, 
								player_list[i].skin_tex_id,
								USE_PLAYERS_SKIN);
		
					if(player_list[i].draw_external_wep == TRUE)
					{
						curr_wep = player_list[i].current_weapon;

						PlayerToD3DVertList(other_players_guns[curr_wep].model_id, 
								current_frame, angle, 
								other_players_guns[curr_wep].skin_tex_id,
								USE_PLAYERS_SKIN);
					}

					if(player_list[i].bIsFiring == TRUE)
					{
						AddPlayerLightSource(i, wx, wy, wz);
					}
				}
			}
			else
			{
				angle = 180 - (int)player_list[i].rot_angle;
			
				wy -= 12;

				if( (abs((int)(wx - player_x) < 512)) && (abs((int)(wz - player_z) < 512)) ) 
				{
					curr_car = player_list[i].current_car;
					PlayerToD3DVertList(car_list[curr_car].model_id, 
								current_frame, angle, 
								car_list[curr_car].skin_tex_id,
								USE_DEFAULT_MODEL_TEX);
				}
			
			
			}
		}
	}


	// DRAW DEBUG MODELS
	
	x_off = 0;
	y_off = 0;
	z_off = 0;

	for(i = 0; i < num_debug_models; i++)
	{
		wx = debug[i].x; 
		wy = debug[i].y;		
		wz = debug[i].z;
		angle = (int)debug[i].rot_angle;

		if( (abs((int)(wx - player_x)) < 512) && (abs((int)(wz - player_z)) < 512) ) 
		{
			use_player_skins_flag = 1;
			current_frame = debug[i].current_frame;

			PlayerToD3DVertList(debug[i].model_id, 
						current_frame,
						angle, 
						debug[i].skin_tex_id,
						USE_DEFAULT_MODEL_TEX);
		}
	}

	if(RR_exit_debug_flag == TRUE)
		RR_exit_debug_flag = TRUE;
	

	if(number_of_polys_per_frame == 0)
	{
		PrintMessage(NULL, "number_of_polys_per_frame = 0", NULL, LOGFILE_ONLY);
		return TRUE;
	}
    car_speed=(float)0;

 
    hr = m_pd3dDevice->BeginScene();
	if (hr != D3D_OK)
	{
		PrintMessage(NULL, "BeginScene : FAILED", NULL, LOGFILE_ONLY);
        return FALSE;
	}
	
	if (m_pd3dDevice->SetClipStatus(&status) != D3D_OK)
	{
		PrintMessage(NULL, "SetClipStatus : FAILED", NULL, LOGFILE_ONLY);
        return FALSE; 
	}

	if (m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE)!= D3D_OK) 
		return FALSE;

	///////////////////////////////////////////////////////////////////////
	//
	// If the fog is not exponential,
    //		Draw the cloud background we will animate
    //
	if (m_tableFog == FALSE)
	{
		m_pd3dDevice->SetTexture( 0, D3DTextr_GetSurface("..\\textures\\Cloud3.bmp") );
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                                 m_pBackground, 4, 0 );
	}
	else
	{
		// 
		// Set the background to blue
		//
		m_pd3dDevice->Clear( 0, 
							 NULL, 
							 D3DCLEAR_TARGET, 
							 RGBA_MAKE( 0, 0, 80, 0 ), 
							 1.0f, 
							 0L );
	}

	last_texture_number = 1000;
	vert_index = 0;
	face_index = 0;
	ap_cnt = 0;

	num_polys_per_frame = number_of_polys_per_frame;

	
	for(i = 0; i < number_of_polys_per_frame; i++)
	{
		texture_alias_number = texture_list_buffer[i];
		texture_number = TexMap[texture_alias_number].texture; 

		if(texture_number >= NumTextures)
			texture_number = 0;

		lpDDsurface = lpddsImagePtr[texture_number];

		if( (TexMap[texture_alias_number].is_alpha_texture == TRUE) &&
			(bEnableAlphaTransparency == TRUE) )
		{
			alpha_poly_index[ap_cnt] = i;
			alpha_vert_index[ap_cnt] = vert_index;
			alpha_face_index[ap_cnt] = face_index;
			vert_index += verts_per_poly[i];
			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
				face_index += faces_per_poly[i]*3;
			ap_cnt++;
		} 
		else
		{
			if(last_texture_number != texture_number)
			{
				if(lpDDsurface == NULL)
					PrintMessage(NULL,"Can't find texture", NULL, LOGFILE_ONLY);
				else
				{
					if(m_pd3dDevice->SetTexture( 0, lpDDsurface) != DD_OK )
						PrintMessage(NULL,"SetTexture FAILED", NULL, LOGFILE_ONLY);
				}
			}

			if(dp_command_index_mode[i] == USE_NON_INDEXED_DP) 
			{
				if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX, 
					(LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}
				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];
			}

			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
			{
				dwIndexCount  = faces_per_poly[i] * 3;
				dwVertexCount = verts_per_poly[i];
				command		  = dp_commands[i];

				last_texture_number = texture_number;

				for(t = 0; t < (int)dwIndexCount; t++)
				{
					f_index = src_f[face_index + t];

					memset(&temp_v[t], 0, sizeof(D3DVERTEX));
					memcpy(&temp_v[t], &src_v[vert_index + f_index], 
						sizeof(D3DVERTEX));
				}

				if (m_pd3dDevice->DrawPrimitive(command, 
								D3DFVF_VERTEX, 
								(LPVOID)&temp_v[0], 
								dwIndexCount, 
								NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;
			}
		}
	} // end for i


	if(bEnableAlphaTransparency)
	{
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, 
                            D3DBLEND_SRCCOLOR); 
		
		m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, 
                          D3DBLEND_INVSRCCOLOR); 


		for(j = 0; j < ap_cnt; j++)
		{
			i = alpha_poly_index[j];
			vert_index = alpha_vert_index[j];
			face_index = alpha_face_index[j];
			texture_alias_number = texture_list_buffer[i];
			texture_number = TexMap[texture_alias_number].texture; 

			if(texture_number >= NumTextures)
				texture_number = 0;

			lpDDsurface = lpddsImagePtr[texture_number];

		
			if(last_texture_number != texture_number)
			{
				if(lpDDsurface == NULL)
					PrintMessage(NULL,"Can't find texture", NULL, LOGFILE_ONLY);
				else
				{
					if(m_pd3dDevice->SetTexture( 0, lpDDsurface) != DD_OK )
						PrintMessage(NULL,"SetTexture FAILED", NULL, LOGFILE_ONLY);
				}
			}

			if(dp_command_index_mode[i] == USE_NON_INDEXED_DP) 
			{
				if (m_pd3dDevice->DrawPrimitive(dp_commands[i], D3DFVF_VERTEX, 
					(LPVOID)&src_v[vert_index], verts_per_poly[i], NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}
				last_texture_number = texture_number;
				vert_index += verts_per_poly[i];
			}

			if(dp_command_index_mode[i] == USE_INDEXED_DP) 	
			{
				dwIndexCount  = faces_per_poly[i] * 3;
				dwVertexCount = verts_per_poly[i];
				command		  = dp_commands[i];

				last_texture_number = texture_number;

				for(t = 0; t < (int)dwIndexCount; t++)
				{
					f_index = src_f[face_index + t];

					memset(&temp_v[t], 0, sizeof(D3DVERTEX));
					memcpy(&temp_v[t], &src_v[vert_index + f_index], 
						sizeof(D3DVERTEX));
				}

				if (m_pd3dDevice->DrawPrimitive(command, 
								D3DFVF_VERTEX, 
								(LPVOID)&temp_v[0], 
								dwIndexCount, 
								NULL) != D3D_OK) 
				{
					PrintMessage(NULL, "CMyD3DApplication::Render - DrawPrimitive FAILED", NULL, LOGFILE_ONLY);		
					return FALSE;
				}

				last_texture_number = texture_number;

				face_index += dwIndexCount;
				vert_index += dwVertexCount;
			}
		} // end for i
		
	} // end if

	if(bInWalkMode)
		RrBltCrossHair(lpddsImagePtr[1]); 

	if(RRAppActive)
	{
		if(bInTalkMode)
			OutputText(20, 40, rr_multiplay_chat_string );
	}

	if(bInTalkMode == TRUE)
		display_scores = FALSE;
	else
		DisplayScores();

	if(display_scores == FALSE)
		DisplayIncomingDpChatMessages();

	num_verts_per_frame = vert_index;

	if(rendering_first_frame == TRUE)
	{
		itoa(number_of_polys_per_frame, buffer, 10);
				PrintMessage(NULL, "num_poly = ", buffer, LOGFILE_ONLY);	
	}

    if (m_pd3dDevice->GetClipStatus(&status) != D3D_OK)
	{
		PrintMessage(NULL, "GetClipStatus : FAILED", NULL, LOGFILE_ONLY);
        return FALSE; 
	}
		
	hr = m_pd3dDevice->EndScene();
	if (hr != D3D_OK)
	{
		PrintMessage(NULL, "EndScene : FAILED", NULL, LOGFILE_ONLY);
        return FALSE;
	}

	rendering_first_frame = FALSE;

    return S_OK;
}





int CMyD3DApplication::CheckValidTextureAlias(HWND hwnd, char *alias)
{
	int i;
	char *buffer2;

	for(i = 0; i < number_of_tex_aliases; i++)
	{
		buffer2 = TexMap[i].tex_alias_name;

		if(strcmpi(buffer2, alias)==0)
		{
			return i;
		}
	}
	MessageBox(hwnd, alias, NULL, MB_OK);
		
	return -1; //error
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // Size the background
    D3DVIEWPORT7 vp;
    m_pd3dDevice->GetViewport(&vp);
    FLOAT fAspect = ((FLOAT)vp.dwHeight) / vp.dwWidth;
    m_pBackground[0].sy = (FLOAT)((vp.dwHeight / 2.0) + 30.0);
    m_pBackground[2].sy = (FLOAT)((vp.dwHeight / 2.0) + 30.0);
    m_pBackground[2].sx = (FLOAT)vp.dwWidth;
    m_pBackground[3].sx = (FLOAT)vp.dwWidth;

    // Create and set up the object material
    D3DMATERIAL7 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 0.0f );
    mtrl.power = 40.0f;
    m_pd3dDevice->SetMaterial( &mtrl );
    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, 0x00505050);
    
	// Set the transform matrices
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Turn on lighting. Light will be set during FrameMove() call
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, bEnableLighting );

	if (!pCWorld->LoadRRTextures(m_hWnd, "textures.dat"))
		PrintMessage(m_hWnd, "LoadRRTextures failed", NULL, LOGFILE_ONLY);

    // Set miscellaneous renderstates
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE); 
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE); 	
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
	
    // Set texture renderstates	
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_POINT); 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTFP_NONE );
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    D3DTextr_InvalidateAllTextures();
    return S_OK;
}



void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode)
{
	FILE *fp;
	char tmessage[100];

	
	if((message1 == NULL) && (message2 == NULL))
		return;

	if((message1 != NULL) && (message2 != NULL))
	{
		strcpy(tmessage, message1);
		strcat(tmessage, message2);
	}
	else
	{
		if(message1 != NULL) 
			strcpy(tmessage, message1);

		if(message2 != NULL) 
			strcpy(tmessage, message2);
	}


	if(logfile_start_flag == TRUE)
	{
		fp = fopen("rrlogfile.txt","w");
		fprintf( fp, "%s\n\n", "RR Logfile");
	}
	else
		fp = fopen("rrlogfile.txt","a");

	logfile_start_flag = FALSE;
    
	if(fp == NULL)
    {     
        MessageBox(hwnd,"Can't load logfile","File Error",MB_OK);
		fclose(fp);
		return;
	}

	fprintf( fp, " %s\n", tmessage );

	

	if(message_mode != LOGFILE_ONLY)
	{
		hdc=GetDC(hwnd);
		TextOut(hdc,PrintMsgX,PrintMsgY, tmessage,strlen(tmessage));
		PrintMsgY +=20;
		ReleaseDC(hwnd,hdc);
	}
	
	fclose(fp);
	
}



void PrintMemAllocated(int mem, char *message)
{
	FILE *fp;
	char buffer[100];
	char buffer2[100];
	int mem_kb;

	if(logfile_start_flag == TRUE)
	{
		fp = fopen("rrlogfile.txt","w");
		fprintf( fp, "%s\n\n", "RR Logfile");
	}
	else
		fp = fopen("rrlogfile.txt","a");

	logfile_start_flag = FALSE;
    
	if(fp == NULL)
    {     
        MessageBox(NULL,"Can't load logfile","File Error",MB_OK);
		fclose(fp);
		return;
	}

	strcpy(buffer, "memory allocated for ");
	strcat(buffer, message);
	strcat(buffer, " = ");

	mem_kb = mem / 1024;
	itoa(mem_kb, buffer2, 10);
	strcat(buffer, buffer2);
	strcat(buffer, " KB");

	fprintf( fp, " %s\n", buffer );

	total_allocated_memory_count += mem;
	
	fclose(fp);
	
}



HRESULT CMyD3DApplication::AnimateCharacters()
{
	int i;

	for(i = 0; i < num_players; i++)
	{			
		mod_id	    = player_list[i].model_id;
		curr_frame  = player_list[i].current_frame;
		stop_frame  = pmdata[mod_id].sequence_stop_frame[player_list[i].current_sequence];
				
		if(player_list[i].bStopAnimating == FALSE)
		{
			if(curr_frame >= stop_frame)
			{	
				if(player_list[i].health > 0)
				{
					player_list[i].current_sequence = 0;
					curr_seq = player_list[i].current_sequence;
					player_list[i].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];
				}
				else
					player_list[i].bStopAnimating = TRUE;
			}
			else
				player_list[i].current_frame++;
		}
	}

	mod_id	    = your_gun[current_gun].model_id;
	curr_frame  = your_gun[current_gun].current_frame;
	stop_frame  = pmdata[mod_id].sequence_stop_frame[your_gun[current_gun].current_sequence];
					
	if(curr_frame >= stop_frame)
	{	
		your_gun[current_gun].current_sequence = 2;
		curr_seq = your_gun[current_gun].current_sequence;
		your_gun[current_gun].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];
	}
				
	your_gun[current_gun].current_frame++;
				
	// Animate Weapons

	// Animate Your Debug models

			
	for(i = 0 ; i < num_debug_models; i++)
	{					
		mod_id	    = debug[i].model_id;
		curr_frame  = debug[i].current_frame;
		stop_frame  = pmdata[mod_id].sequence_stop_frame[debug[i].current_sequence];
					
		if(curr_frame >= stop_frame)
		{	
			debug[i].current_sequence = 0;
			curr_seq = debug[i].current_sequence;
			debug[i].current_frame = pmdata[mod_id].sequence_start_frame[curr_seq];
		}
		else
			debug[i].current_frame++;
	}

	return 0;
}

void CMyD3DApplication::SetPlayerAnimationSequence(int player_number, int sequence_number)
{
	int model_id;
	int start_frame;

	if(player_number >= num_players)
	{
		PrintMessage(NULL,"SetPlayerAnimationSequence player_number too large FAILED", NULL, LOGFILE_ONLY);
			return;
	}
	player_list[player_number].current_sequence = sequence_number;
	
	if(sequence_number > 6)
	{
		PrintMessage(NULL,"SetPlayerAnimationSequence - sequence_number too large FAILED", NULL, LOGFILE_ONLY);
			return;
	}
	model_id	= player_list[player_number].model_id;

	if(model_id > MAX_NUM_PLAYERS)
	{
		PrintMessage(NULL,"SetPlayerAnimationSequence - model_id too large FAILED", NULL, LOGFILE_ONLY);
			return;
	}

	start_frame = pmdata[model_id].sequence_start_frame[sequence_number];	
	player_list[player_number].current_frame = start_frame;
}


BOOL CALLBACK FireWeapon(UINT uTimerID, 
		UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)         
{
	int i;
	char buffer[256];
   
	pCMyApp->firing_gun_flag = TRUE;
		
	pCMyApp->StopBuffer(TRUE,0);
	pCMyApp->PlaySound(0, FALSE);

	if(pCMyApp->num_players == 0)
		return TRUE;

	i = GetPlayerNumber(pCMyApp->MyRRnetID);

	if( (i >= 0) && (i < MAX_NUM_PLAYERS) )
	{
		pCMyApp->player_list[i].x = pCMyApp->m_vEyePt.x; 
		pCMyApp->player_list[i].y = pCMyApp->m_vEyePt.y;		
		pCMyApp->player_list[i].z = pCMyApp->m_vEyePt.z;
		pCMyApp->player_list[i].bIsFiring = TRUE;

		SendActionWordMessage(pCMyApp->MyRRnetID, pCMyApp->current_gun,  APPMSG_FIRE);
		
		pCMyApp->IsPlayerHit();
	}
	else
	{
		itoa(pCMyApp->MyRRnetID, buffer, 10);
		PrintMessage(NULL,"MyRRnetID = ", buffer, LOGFILE_ONLY);

		itoa(i, buffer, 10);
		PrintMessage(NULL,"i = ", buffer, LOGFILE_ONLY);
		
		PrintMessage(NULL,"IsPlayerHit - FireWeapon FAILED", NULL, LOGFILE_ONLY);
		return FALSE;
	}

	return TRUE;
}

void CMyD3DApplication::DisplayScores()
{
	int i;
	int offset_y = 40;
	HDC hdc;
	char buf[256];
	char buf2[256];
	HRESULT LastError;

	if(display_scores == FALSE)
		return;

	if(GetFramework()->GetBackBuffer() == NULL)
		return;

	LastError = GetFramework()->GetBackBuffer()->GetDC(&hdc);

	if (LastError == DD_OK) 
	{	
		SetTextColor(hdc, RGB(255,255,0));
		SetBkColor(hdc, RGB(0,0,0));
		SetBkMode(hdc, TRANSPARENT);
	
		TextOut(hdc,  50, offset_y, "Scores", 6);
		offset_y += 20;

		TextOut(hdc,  50, offset_y, "No.", 3);
		TextOut(hdc,  90, offset_y, "Name", 4);
		TextOut(hdc, 300, offset_y, "Frags", 5);
		TextOut(hdc, 345, offset_y, "RRnetID", 7);
		TextOut(hdc, 450, offset_y, "Ping", 4);
	
		offset_y += 20;

		for(i = 50; i < 480; i += 10)
			TextOut(hdc, i, offset_y, "--", 2);

		offset_y += 20;

		i = 0;

		for(i = 0; i < num_players; i++)
		{
			if(player_list[i].bIsPlayerValid == TRUE)
			{
				itoa(i,buf,10);
				TextOut(hdc, 50, offset_y, buf, strlen(buf));

				strcpy(buf, player_list[i].name);
				TextOut(hdc, 90, offset_y, buf, strlen(buf));
			
				itoa(player_list[i].frags, buf,10);
				TextOut(hdc, 300, offset_y, buf,strlen(buf));

				itoa((int)player_list[i].RRnetID, buf,10);
				TextOut(hdc, 350, offset_y, buf,strlen(buf));
			
				itoa(player_list[i].ping, buf,10);
				TextOut(hdc, 450, offset_y, buf,strlen(buf));

				offset_y += 20;
			}
		}
		offset_y+=20;
	
		
		for(i = 50; i < 480; i += 10)
			TextOut(hdc, i, offset_y, "--", 2);

		strcpy(buf, "Rx = ");
		itoa(num_packets_received_ps, buf2, 10);
		strcat(buf, buf2);
		strcat(buf, "     Tx = ");
		itoa(num_packets_sent_ps, buf2, 10);
		strcat(buf, buf2);

		offset_y += 20;
		TextOut(hdc, 50, offset_y, buf, strlen(buf));
		
	}
	else
        PrintMessage(NULL, "DisplayScores() - GetDC FAILED", NULL, NULL);

	GetFramework()->GetBackBuffer()->ReleaseDC(hdc);
}
