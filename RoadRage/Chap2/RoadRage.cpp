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
#include "D3DApp.h"
#include "RoadRage.hpp"

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------

#define EXITCODE_FORWARD     1  // Dialog success, so go forward
#define EXITCODE_BACKUP      2  // Dialog canceled, show previous dialog
#define EXITCODE_QUIT        3  // Dialog quit, close app
#define EXITCODE_ERROR       4  // Dialog error, terminate app


extern BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM );

FLOAT                g_fCurrentTime;
extern GUID                 g_AppGUID; 
extern TCHAR                g_strAppName[256];
		
HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;

CMyD3DApplication* pCMyApp;

void PrintMemAllocated(int mem, char *message);
VOID DisplayError( CHAR* strMessage );


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
    m_strWindowTitle  = TEXT( "Chapter 2" );

	pCMyApp = this;
}


LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam )
{
	HMENU hMenu;

	m_hWnd = hWnd;
		
	hMenu = GetMenu( hWnd );

    switch( uMsg )
    {
        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
				case MENU_ABOUT:
					DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
                    break;

                case IDM_EXIT:
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
            DestroyWindow( hWnd );
            PostQuitMessage(0);
            return 0;

    	default:
			return CD3DApplication::MsgProc( hWnd, uMsg, wParam,
											 lParam );
	}

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}  


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	
	CMyD3DApplication d3dApp;

	d3dApp.hInstApp = hInst;

    if( FAILED( d3dApp.Create( hInst, strCmdLine ) ) )
        return 0;

    d3dApp.Run();		
	
	CoUninitialize();
	return TRUE;
}


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------