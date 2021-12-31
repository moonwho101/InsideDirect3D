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
    m_strWindowTitle  = TEXT( "Chapter 4" );
    m_bAppUseZBuffer  = TRUE;
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
		case WM_INITMENUPOPUP:
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


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
	}

    // End the scene.
    m_pd3dDevice->EndScene();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // Set the transform matrices
    D3DMATRIX matWorld, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

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
    return S_OK;
}


