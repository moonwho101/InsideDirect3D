//-----------------------------------------------------------------------------
// File: D3DApp.cpp
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) 1998-1999 Microsoft Corporation. All rights reserved.
//				 Additions by Peter Kovach and William Chin
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "D3DApp.h"

//-----------------------------------------------------------------------------
// Internal function prototypes and variables
//-----------------------------------------------------------------------------
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOSOFTWARE };

static INT     CALLBACK AboutProc( HWND, UINT, WPARAM, LPARAM );
static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

CD3DApplication* g_pD3DApp;

//-----------------------------------------------------------------------------
// Name: CD3DApplication()
// Desc: This is the constructor for CD3DApplication 
//-----------------------------------------------------------------------------
CD3DApplication::CD3DApplication()
{
    m_hWnd         = NULL;

    m_bActive         = FALSE;
    m_bReady          = FALSE;

    m_strWindowTitle  = _T("Direct3D Application");

    g_pD3DApp = this;
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: This method selects a D3D device, initializes our custom scene
//		 content (which is a lot for RoadRage), 
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Create( HINSTANCE hInst, TCHAR* strCmdLine )
{
    // Register the window class
    WNDCLASS wndClass = { 0, WndProc, 0, 0, hInst,
                          LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN_ICON) ),
                          LoadCursor( NULL, IDC_ARROW ), 
                          (HBRUSH)GetStockObject(WHITE_BRUSH),
                          NULL, _T("D3D Window") };
    RegisterClass( &wndClass );

    // Create the render window
    m_hWnd = CreateWindow( _T("D3D Window"), m_strWindowTitle,
                           WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                           CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0L,
                           LoadMenu( hInst, MAKEINTRESOURCE(IDR_MENU) ), 
                           hInst, 0L );

	if (!m_hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		//
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		//
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}

    UpdateWindow( m_hWnd );

    // The app is ready to go
    m_bReady = TRUE;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Run()
// Desc: Message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT CD3DApplication::Run()
{
    // Load keyboard accelerators
    HACCEL hAccel = LoadAccelerators( NULL, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

    // Now we're ready to recieve and process Windows messages.
    BOOL bGotMsg;
    MSG  msg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
        if( m_bActive )
            bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        else
            bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            if( 0 == TranslateAccelerator( m_hWnd, hAccel, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
    }

    return msg.wParam;
}


//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( g_pD3DApp )
        return g_pD3DApp->MsgProc( hWnd, uMsg, wParam, lParam );

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM )
{
    if( WM_COMMAND == uMsg )
        if( IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam) )
            EndDialog( hWnd, TRUE );

    return WM_INITDIALOG == uMsg ? TRUE : FALSE;
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message handling function.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_PAINT:
            // Handle paint messages when the app is not ready
            break;

        case WM_MOVE:
            // If in windowed mode, move the Direct3D Framework's window
            break;

        case WM_SIZE:
            // Check to see if we are losing our window...
            if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
                m_bActive = FALSE;
            else
                m_bActive = TRUE;

        case WM_SETCURSOR:
            // Prevent a cursor in fullscreen mode
            break;

        case WM_ENTERMENULOOP:
            break;
        case WM_EXITMENULOOP:
            break;

        case WM_ENTERSIZEMOVE:
            // Halt frame movement while the app is sizing or moving
            break;
        case WM_EXITSIZEMOVE:
            break;

        case WM_CONTEXTMENU:
            // Handle the app's context menu (via right mouse click) 
            TrackPopupMenuEx(
                    GetSubMenu( LoadMenu( 0, MAKEINTRESOURCE(IDR_POPUP) ), 0 ),
                    TPM_VERTICAL, LOWORD(lParam), HIWORD(lParam), hWnd, NULL );
            break;

        case WM_NCHITTEST:
            // Prevent the user from selecting the menu in fullscreen mode
            break;

        case WM_POWERBROADCAST:
            switch( wParam )
            {
                case PBT_APMQUERYSUSPEND:
                    // At this point, the app should save any data for open
                    // network connections, files, etc.., and prepare to go into
                    // a suspended mode.
                    return OnQuerySuspend( (DWORD)lParam );

                case PBT_APMRESUMESUSPEND:
                    // At this point, the app should recover any data, network
                    // connections, files, etc.., and resume running from when
                    // the app was suspended.
                    return OnResumeSuspend( (DWORD)lParam );
            }
            break;

        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    break;
            }
            break;

        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
               case IDM_ABOUT:
                    // Display the About box
                    DialogBox( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
                               MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutProc );
                    return 0;

                case IDM_EXIT:
                    // Recieved key/menu command to exit app
                    SendMessage( hWnd, WM_CLOSE, 0, 0 );
                    return 0;
            }
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

		// Close the window
        case WM_CLOSE:
            DestroyWindow( hWnd );
            return 0;

		// The WM_DESTROY message is sent when a window is being 
		// destroyed. It is sent to the window procedure of the window 
		// being destroyed after the window is removed from the screen. 
		//
		// This message is sent first to the window being destroyed and 
		// then to the child windows (if any) as they are destroyed. 
		// During the processing of the message, it can be assumed that 
		// all child windows still exist. 
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
	//
	// The DefWindowProc function calls the default window procedure 
	// to provide default processing for any window messages that an 
	// application does not process. This function ensures that every 
	// message is processed. DefWindowProc is called with the same 
	// parameters received by the window procedure. 

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
            

//-----------------------------------------------------------------------------
// Name: OnQuerySuspend()
// Desc: Called when the app receives a PBT_APMQUERYSUSPEND message, meaning
//       the computer is about to be suspended. At this point, the app should
//       save any data for open network connections, files, etc.., and prepare
//       to go into a suspended mode.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::OnQuerySuspend( DWORD dwFlags )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: OnResumeSuspend()
// Desc: Called when the app receives a PBT_APMRESUMESUSPEND message, meaning
//       the computer has just resumed from a suspended state. At this point, 
//       the app should recover any data, network connections, files, etc..,
//       and resume running from when the app was suspended.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::OnResumeSuspend( DWORD dwData )
{
    return TRUE;
}


