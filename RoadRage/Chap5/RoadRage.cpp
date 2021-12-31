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
		
BOOL logfile_start_flag = TRUE;
BOOL RR_exit_debug_flag = FALSE;
SETUPINFO setupinfo;
HDC hdc;

int PrintMsgX = 10;
int PrintMsgY = 10;

int num_op_guns = 0;
int num_cars;

int total_allocated_memory_count = 0;
int num_light_sources;
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
    m_strWindowTitle  = TEXT( "Chapter 5" );
    m_bAppUseZBuffer  = TRUE;
    m_fnConfirmDevice = NULL;


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
		case WM_ACTIVATEAPP:
			RRAppActive = (BOOL)wParam;
            break;
		
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
                    PrintMessage(NULL, "MsgProc - IDM_EXIT", NULL, LOGFILE_ONLY);
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

	PrintMessage(NULL, "Quitting", NULL, LOGFILE_ONLY);
		
	
	CoUninitialize();
	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    WORD i, j, ind, v;

    // Seed the random number generator
    srand( time(0) );

    // Generate a square mesh in XZ plane from 0,0 to 1,1 for the walls
    for( i=0; i<WALL_MESH_SIZE; i++ )
    {
        for( j=0; j<WALL_MESH_SIZE; j++ )
        {
            FLOAT      x = i / (FLOAT)(WALL_MESH_SIZE-1);
            FLOAT      z = j / (FLOAT)(WALL_MESH_SIZE-1);
            D3DVERTEX* v = &m_WallVertices[i*WALL_MESH_SIZE+j];
            (*v) = D3DVERTEX( 10.0f*D3DVECTOR(x,0.0f,z),
                              D3DVECTOR(0.0f,1.0f,0.0f), x, z );
        }
    }

    // Generate the wall indices
    for( i=ind=0; i<WALL_MESH_SIZE-1; i++ )
    {
        for( j=0; j<WALL_MESH_SIZE-1; j++ )
        {
            m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+0);
            m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+1);
            m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+0);
            m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+0);
            m_WallIndices[ind++] = (i+0)*WALL_MESH_SIZE + (j+1);
            m_WallIndices[ind++] = (i+1)*WALL_MESH_SIZE + (j+1);
        }
    }

    // Generate the sphere data
    FLOAT dj = g_PI/(SPHERE_MESH_SIZE+1.f);
    FLOAT di = g_PI/SPHERE_MESH_SIZE;

    // Vertices 0 and 1 are the north and south poles
    m_SphereVertices[0] = D3DVERTEX( D3DVECTOR(0.0f, 1.0f, 0.0f), 
                           D3DVECTOR(0.0f,-1.0f, 0.0f), rnd(), rnd() );
    m_SphereVertices[1] = D3DVERTEX( D3DVECTOR(0.0f, -1.0f, 0.0f), 
                           D3DVECTOR(0.0f, 1.0f, 0.0f), rnd(), rnd() );

    for( j=0; j<SPHERE_MESH_SIZE; j++ )
    {
        for( i=0; i<SPHERE_MESH_SIZE*2; i++ ) 
        {
            D3DVECTOR   p;

            p.y = (FLOAT)( cos((j+1) * dj) );
            p.x = (FLOAT)( sin(i * di) * sin((j+1) * dj) );
            p.z = (FLOAT)( cos(i * di) * sin((j+1) * dj) );
            m_SphereVertices[2+i+j*SPHERE_MESH_SIZE*2] = D3DVERTEX(p, -p, rnd(), rnd());
        }
    }

    // Now generate the traingle indices. Strip around north pole first
    for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
    {
        m_SphereIndices[3*i+0] = 0;
        m_SphereIndices[3*i+1] = i+2;
        m_SphereIndices[3*i+2] = i+3;
        if( i==SPHERE_MESH_SIZE*2-1 )
            m_SphereIndices[3*i+2] = 2;
    }

    // Now all the middle strips
    for( j=0; j<SPHERE_MESH_SIZE-1; j++ )
    {
        v = 2+j*SPHERE_MESH_SIZE*2;
        ind = 3*SPHERE_MESH_SIZE*2 + j*6*SPHERE_MESH_SIZE*2;
        for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
        {
            m_SphereIndices[6*i+0+ind] = v+i;
            m_SphereIndices[6*i+2+ind] = v+i+1;
            m_SphereIndices[6*i+1+ind] = v+i+SPHERE_MESH_SIZE*2;

            m_SphereIndices[6*i+0+ind+3] = v+i+SPHERE_MESH_SIZE*2;
            m_SphereIndices[6*i+2+ind+3] = v+i+1;
            m_SphereIndices[6*i+1+ind+3] = v+i+SPHERE_MESH_SIZE*2+1;
            if( i==SPHERE_MESH_SIZE*2-1 )
            {
                m_SphereIndices[6*i+2+ind+0] = v+i+1-2*SPHERE_MESH_SIZE;
                m_SphereIndices[6*i+2+ind+3] = v+i+1-2*SPHERE_MESH_SIZE;
                m_SphereIndices[6*i+1+ind+3] = v+i+SPHERE_MESH_SIZE*2+1-2*SPHERE_MESH_SIZE;
            }
        }
    }

    // Finally strip around south pole
    v   = NUM_SPHERE_VERTICES-SPHERE_MESH_SIZE*2;
    ind = NUM_SPHERE_INDICES-3*SPHERE_MESH_SIZE*2;
    for( i=0; i<SPHERE_MESH_SIZE*2; i++ )
    {
        m_SphereIndices[3*i+0+ind] = 1;
        m_SphereIndices[3*i+1+ind] = v+i+1;
        m_SphereIndices[3*i+2+ind] = v+i;
        if( i==SPHERE_MESH_SIZE*2-1 )
            m_SphereIndices[3*i+1+ind] = v;
    }

    return S_OK;
}







//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove( FLOAT fTimeKey )
{
    // Rotate through the various lights types
    m_dltType = (D3DLIGHTTYPE)(1+(((DWORD)fTimeKey)/5)%3);

    // Make sure light is supported by the device
    DWORD dwCaps = m_pDeviceInfo->ddDeviceDesc.dwVertexProcessingCaps;

    if( 0 == ( dwCaps & D3DVTXPCAPS_POSITIONALLIGHTS ) )
    {
        if( m_dltType == D3DLIGHT_POINT || m_dltType == D3DLIGHT_SPOT )
            m_dltType = D3DLIGHT_DIRECTIONAL;
    }

    // Values for the light position, direction, and color
    FLOAT x = sinf( fTimeKey*2.000f );
    FLOAT y = sinf( fTimeKey*2.246f );
    FLOAT z = sinf( fTimeKey*2.640f );

    // Set up the light structure
    D3DLIGHT7 light;
    ZeroMemory( &light, sizeof(light) );
    light.dltType       = m_dltType;
    light.dcvDiffuse.r  = 0.5f + 0.5f * x;
    light.dcvDiffuse.g  = 0.5f + 0.5f * y;
    light.dcvDiffuse.b  = 0.5f + 0.5f * z;
    light.dvRange       = D3DLIGHT_RANGE_MAX;
    
    switch( m_dltType )
    {
        case D3DLIGHT_POINT:
            light.dvPosition     = 4.5f * D3DVECTOR( x, y, z );
            light.dvAttenuation1 = 0.4f;
            break;
        case D3DLIGHT_DIRECTIONAL:
            light.dvDirection    = D3DVECTOR( x, y, z );
            break;
        case D3DLIGHT_SPOT:
            light.dvDirection    = D3DVECTOR( x, y, z );
            light.dvFalloff      = 100.0f;
            light.dvTheta        =   0.8f;
            light.dvPhi          =   1.0f;
            light.dvAttenuation0 =   1.0f;
    }

    // Set the light
    m_pd3dDevice->SetLight( 0, &light );

    // Move the camera position around
    FLOAT     toc = 0.3f*x - g_PI/4;
    D3DVECTOR vFrom( sinf(toc)*4.0f, 3.0f, -cosf(toc)*4.0f );
    D3DVECTOR vAt( 0.0f, 0.0f, 0.0f );
    D3DVECTOR vUp( 0.0f, 1.0f, 0.0f );
    SetViewParams( &vFrom, &vAt, &vUp, 0.1f );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define WALL_MESH_SIZE      12
#define NUM_WALL_VERTICES   (WALL_MESH_SIZE*WALL_MESH_SIZE)
#define NUM_WALL_INDICES    ((WALL_MESH_SIZE-1)*(WALL_MESH_SIZE-1)*6)

#define SPHERE_MESH_SIZE    4
#define NUM_SPHERE_VERTICES (2+SPHERE_MESH_SIZE*SPHERE_MESH_SIZE*2)
#define NUM_SPHERE_INDICES  ( (SPHERE_MESH_SIZE*4 + SPHERE_MESH_SIZE*4* \
                              (SPHERE_MESH_SIZE-1) ) * 3 )

#define rnd() ( ( ((FLOAT)rand())-((FLOAT)rand()) ) / RAND_MAX )




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
        D3DMATRIX matWorld, matTrans, matRotate;

         // Draw the bottom wall
        D3DUtil_SetTranslateMatrix( matTrans,-5.0f, -5.0f, -5.0f );
        D3DUtil_SetRotateZMatrix( matRotate, 0.0f );
        D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
        m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld );
        m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                            m_WallVertices, NUM_WALL_VERTICES,
                                            m_WallIndices, NUM_WALL_INDICES, 0 );

        // Draw the back wall
        D3DUtil_SetTranslateMatrix( matTrans, 5.0f,-5.0f, -5.0f );
        D3DUtil_SetRotateZMatrix( matRotate, g_PI/2 );
        D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                            m_WallVertices, NUM_WALL_VERTICES,
                                            m_WallIndices, NUM_WALL_INDICES, 0 );

        // Draw the side wall
        D3DUtil_SetTranslateMatrix( matTrans, -5.0f, -5.0f, 5.0f );
        D3DUtil_SetRotateXMatrix( matRotate,  -g_PI/2 );
        D3DMath_MatrixMultiply( matWorld, matRotate, matTrans );
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                            m_WallVertices, NUM_WALL_VERTICES,
                                            m_WallIndices, NUM_WALL_INDICES, 0 );

        // Draw sphere at light's position
        D3DLIGHT7 light;
        m_pd3dDevice->GetLight( 0, &light );
        D3DUtil_SetTranslateMatrix( matWorld, light.dvPosition.x,
                                    light.dvPosition.y, light.dvPosition.z );
        m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
        m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX, 
                                            m_SphereVertices, NUM_SPHERE_VERTICES,
                                            m_SphereIndices, NUM_SPHERE_INDICES, 0 );

        // Output the name of the light type
        switch( m_dltType )
        {
            case D3DLIGHT_POINT:
                OutputText(  0, 20, TEXT("Point Light") );
                break;
            case D3DLIGHT_SPOT:
                OutputText( 0, 20, TEXT("Spot Light") );
                break;
            case D3DLIGHT_DIRECTIONAL:
                OutputText( 0, 20, TEXT("Directional Light") );
                break;
        }
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
    // Create and set up the object material
    D3DMATERIAL7 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set the transform matrices
    D3DMATRIX matWorld, matProj;
    D3DUtil_SetIdentityMatrix( matWorld );
    D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
    m_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Turn on lighting. Light will be set during FrameMove() call
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT,  0x20202020 );

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
