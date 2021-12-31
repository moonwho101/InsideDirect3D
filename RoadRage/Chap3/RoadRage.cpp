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
#include "D3DMath.h"
#include "D3DApp.h"
#include "D3DUtil.h"
#include "D3DEnum.h"
#include "RoadRage.hpp"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

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

int total_allocated_memory_count = 0;
float angle_deg = 0.0f; // debug
float temp_f	= 0.0f; // debug

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
    m_strWindowTitle  = TEXT( "Chapter 3" );
    m_fnConfirmDevice = NULL;
	m_bShowStats = TRUE;

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
	HMENU hMenu;

	m_hWnd = hWnd;
		
	hMenu = GetMenu( hWnd );

    switch( uMsg )
    {
        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
				case MENU_ABOUT:
					Pause(TRUE);
					DialogBox(hInstApp, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AppAbout);
					Pause(FALSE);
                break;

                case IDM_EXIT:
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

HRESULT CMyD3DApplication::Render()
{
    HRESULT hr;
    DDBLTFX ddbltfx;

    // Clear the back buffer for this Screen
    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0; // Black
    hr = m_pddsRenderTarget->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	return hr;
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

    // FrameMove (animate) the scene

    // Render the scene as normal
    if( FAILED( hr = Render() ) )
        return hr;

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