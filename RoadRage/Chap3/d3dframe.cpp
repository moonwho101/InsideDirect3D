//-----------------------------------------------------------------------------
// File: D3DFrame.cpp
//
// Desc: Class functions to implement a Direct3D app framework.
//
// Copyright (c) 1995-1999 by Microsoft, all rights reserved
//-----------------------------------------------------------------------------
#define STRICT
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "D3DFrame.h"
#include "D3DUtil.h"

extern LPDIRECTDRAWSURFACE7 g_pddsBackBuffer;


//-----------------------------------------------------------------------------
// Name: CD3DFramework7()
// Desc: The constructor. Clears static variables
//-----------------------------------------------------------------------------
CD3DFramework7::CD3DFramework7()
{
     m_hWnd           = NULL;
     m_bIsFullscreen  = FALSE;
     m_bIsStereo      = FALSE;
     m_dwRenderWidth  = 0L;
     m_dwRenderHeight = 0L;
     
	 m_pddsFrontBuffer    = NULL;
     m_pddsBackBuffer     = NULL;
     m_pddsBackBufferLeft = NULL;
     
     m_pDD             = NULL;
     m_dwDeviceMemType = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFramework7()
// Desc: The destructor. Deletes all objects
//-----------------------------------------------------------------------------
CD3DFramework7::~CD3DFramework7()
{
    DestroyObjects();
}




//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc: Cleans everything up upon deletion. This code returns an error
//       if any of the objects have remaining reference counts.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::DestroyObjects()
{
    LONG nDD  = 0L; // Number of outstanding DDraw references
    LONG nD3D = 0L; // Number of outstanding D3DDevice references

    if( m_pDD )
    {
        m_pDD->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );
    }

    SAFE_RELEASE( m_pddsBackBuffer );
    SAFE_RELEASE( m_pddsBackBufferLeft );
    SAFE_RELEASE( m_pddsFrontBuffer );

    if( m_pDD )
    {
        // Do a safe check for releasing DDRAW. RefCount must be zero.
        if( 0 < ( nDD = m_pDD->Release() ) )
            DEBUG_MSG( _T("Error: DDraw object is still referenced!") );
    }
    m_pDD = NULL;

    // Return successful, unless there are outstanding DD or D3DDevice refs.
    return ( nDD==0 && nD3D==0 ) ? S_OK : D3DFWERR_NONZEROREFCOUNT;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Creates the internal objects for the framework
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::Initialize( HWND hWnd, GUID* pDriverGUID,
                                    GUID* pDeviceGUID, DDSURFACEDESC2* pMode,
                                    DWORD dwFlags )
{
    HRESULT hr;

    // Check params. Note: A NULL mode is valid for windowed modes only.
    if( ( NULL==hWnd ) || ( NULL==pDeviceGUID ) || 
        ( NULL==pMode && (dwFlags&D3DFW_FULLSCREEN) ) )
        return E_INVALIDARG;

    // Setup state for windowed/fullscreen mode
    m_hWnd          = hWnd;
    m_bIsFullscreen = ( dwFlags & D3DFW_FULLSCREEN ) ? TRUE : FALSE;

    // Create the D3D rendering environment (surfaces, device, viewport, and so forth.)
    if( FAILED( hr = CreateEnvironment( pDriverGUID, pDeviceGUID, pMode,
                                        dwFlags ) ) )
    {
        DestroyObjects();
        return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateEnvironment()
// Desc: Creates the internal objects for the framework
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::CreateEnvironment( GUID* pDriverGUID, GUID* pDeviceGUID,
                                           DDSURFACEDESC2* pMode, DWORD dwFlags )
{
    HRESULT hr;

    // Select the default memory type, for whether the device is HW or SW
    if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice) )
        m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else if( IsEqualIID( *pDeviceGUID, IID_IDirect3DTnLHalDevice) )
        m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else
        m_dwDeviceMemType = DDSCAPS_SYSTEMMEMORY;

    // Create the DDraw object
    hr = CreateDirectDraw( pDriverGUID, dwFlags );
    if( FAILED( hr ) )
        return hr;

    // Create the front and back buffers, and attach a clipper
    if( dwFlags & D3DFW_FULLSCREEN )
        hr = CreateFullscreenBuffers( pMode );
    else
        hr = CreateWindowedBuffers();
    if( FAILED( hr ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateDirectDraw()
// Desc: Create the DirectDraw interface
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::CreateDirectDraw( GUID* pDriverGUID, DWORD dwFlags )
{
    // Create the DirectDraw interface
    if( FAILED( DirectDrawCreateEx( pDriverGUID, (VOID**)&m_pDD,
                                    IID_IDirectDraw7, NULL ) ) )
    {
        DEBUG_MSG( _T("Could not create DirectDraw") );
        return D3DFWERR_NODIRECTDRAW;
    }

    // Set the Windows cooperative level
    DWORD dwCoopFlags = DDSCL_NORMAL;
    if( m_bIsFullscreen )
        dwCoopFlags = DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN;

    // By defualt, set the flag to allow D3D to optimize floating point calcs
    if( 0L == ( dwFlags & D3DFW_NO_FPUSETUP ) )
        dwCoopFlags |= DDSCL_FPUSETUP;

    if( FAILED( m_pDD->SetCooperativeLevel( m_hWnd, dwCoopFlags ) ) )
    {
        DEBUG_MSG( _T("Couldn't set coop level") );
        return D3DFWERR_COULDNTSETCOOPLEVEL;
    }

    // Check that we are NOT in a palettized display. That case will fail,
    // since the Direct3D framework doesn't use palettes.
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    m_pDD->GetDisplayMode( &ddsd );
    if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
        return D3DFWERR_INVALIDMODE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFullscreenBuffers()
// Desc: Creates the primary and (optional) backbuffer for rendering.
//       Windowed mode and fullscreen mode are handled differently.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::CreateFullscreenBuffers( DDSURFACEDESC2* pddsd )
{
    HRESULT hr;

    // Get the dimensions of the screen bounds
    // Store the rectangle which contains the renderer
    SetRect( &m_rcScreenRect, 0, 0, pddsd->dwWidth, pddsd->dwHeight );
    m_dwRenderWidth  = m_rcScreenRect.right  - m_rcScreenRect.left;
    m_dwRenderHeight = m_rcScreenRect.bottom - m_rcScreenRect.top;

    // Set the display mode to the requested dimensions. Check for
    // 320x200x8 modes, and set flag to avoid using ModeX
    DWORD dwModeFlags = 0;

    if( (320==m_dwRenderWidth) && (200==m_dwRenderHeight) &&
        (8==pddsd->ddpfPixelFormat.dwRGBBitCount) )
        dwModeFlags |= DDSDM_STANDARDVGAMODE;

    if( FAILED( m_pDD->SetDisplayMode( m_dwRenderWidth, m_dwRenderHeight,
                                pddsd->ddpfPixelFormat.dwRGBBitCount,
                                pddsd->dwRefreshRate, dwModeFlags ) ) )
    {
        DEBUG_MSG( _T("Can't set display mode") );
        return D3DFWERR_BADDISPLAYMODE;
    }

    // Setup to create the primary surface w/backbuffer
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE |
                             DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;

    // Support for stereoscopic viewing
    if( m_bIsStereo )
    {
        ddsd.ddsCaps.dwCaps  |= DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_STEREOSURFACELEFT;
    }

    // Create the primary surface
    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL ) ) )
    {
        DEBUG_MSG( _T("Error: Can't create primary surface") );
        if( hr != DDERR_OUTOFVIDEOMEMORY )
            return D3DFWERR_NOPRIMARY;
        DEBUG_MSG( _T("Error: Out of video memory") );
        return DDERR_OUTOFVIDEOMEMORY;
    }

    // Get the backbuffer, which was created along with the primary.
    DDSCAPS2 ddscaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
    if( FAILED( hr = m_pddsFrontBuffer->GetAttachedSurface( &ddscaps,
                                                &m_pddsBackBuffer ) ) )
    {
        DEBUG_ERR( hr, _T("Error: Can't get the backbuffer") );
        return D3DFWERR_NOBACKBUFFER;
    }

    // Increment the backbuffer count (for consistency with windowed mode)
    m_pddsBackBuffer->AddRef();

    // Support for stereoscopic viewing
    if( m_bIsStereo )
    {
        // Get the left back buffer, which was created along with the primary.
        DDSCAPS2 ddscaps = { 0, DDSCAPS2_STEREOSURFACELEFT, 0, 0 };
        if( FAILED( hr = m_pddsBackBuffer->GetAttachedSurface( &ddscaps,
                                                    &m_pddsBackBufferLeft ) ) )
        {
            DEBUG_ERR( hr, _T("Error: Can't get the left back buffer") );
            return D3DFWERR_NOBACKBUFFER;
        }
        m_pddsBackBufferLeft->AddRef();
    }

	FILE *fplog = fopen("rrlogfile.txt","a");

	ZeroMemory(&m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT));
	m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	hr = m_pddsBackBuffer->GetPixelFormat(&m_ddpfBackBufferPixelFormat);

	if(hr == DD_OK)
	{		
		fprintf( fplog, "Backbuffer Pixelformat RGB bits  = : ");
		fprintf( fplog, "%d\n",m_ddpfBackBufferPixelFormat.dwRGBBitCount);

		fprintf( fplog, "Backbuffer Pixelformat RGB masks = : ");
		fprintf( fplog, "%d %d %d\n\n",
		m_ddpfBackBufferPixelFormat.dwRBitMask,
		m_ddpfBackBufferPixelFormat.dwGBitMask,
		m_ddpfBackBufferPixelFormat.dwBBitMask);
	}
	else
		fprintf( fplog, "Check on backbuffer pixel format FAILED ");

	
	hr = m_pddsBackBuffer->GetSurfaceDesc( &ddsd );
	
	fclose(fplog);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateWindowedBuffers()
// Desc: Creates the primary and (optional) backbuffer for rendering.
//       Windowed mode and fullscreen mode are handled differently.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::CreateWindowedBuffers()
{
    HRESULT hr;

    // Get the dimensions of the viewport and screen bounds
    GetClientRect( m_hWnd, &m_rcScreenRect );
    ClientToScreen( m_hWnd, (POINT*)&m_rcScreenRect.left );
    ClientToScreen( m_hWnd, (POINT*)&m_rcScreenRect.right );
    m_dwRenderWidth  = m_rcScreenRect.right  - m_rcScreenRect.left;
    m_dwRenderHeight = m_rcScreenRect.bottom - m_rcScreenRect.top;

    // Create the primary surface
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL ) ) )
    {
        DEBUG_MSG( _T("Error: Can't create primary surface") );
        if( hr != DDERR_OUTOFVIDEOMEMORY )
            return D3DFWERR_NOPRIMARY;
        DEBUG_MSG( _T("Error: Out of video memory") );
        return DDERR_OUTOFVIDEOMEMORY;
    }

    // If in windowed-mode, create a clipper object
    LPDIRECTDRAWCLIPPER pcClipper;
    if( FAILED( hr = m_pDD->CreateClipper( 0, &pcClipper, NULL ) ) )
    {
        DEBUG_MSG( _T("Error: Couldn't create clipper") );
        return D3DFWERR_NOCLIPPER;
    }

    // Associate the clipper with the window
    pcClipper->SetHWnd( 0, m_hWnd );
    m_pddsFrontBuffer->SetClipper( pcClipper );
    SAFE_RELEASE( pcClipper );

    // Create a backbuffer
    ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    ddsd.dwWidth        = m_dwRenderWidth;
    ddsd.dwHeight       = m_dwRenderHeight;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

    if( FAILED( hr = m_pDD->CreateSurface( &ddsd, &m_pddsBackBuffer, NULL ) ) )
    {
        DEBUG_ERR( hr, _T("Error: Couldn't create the backbuffer") );
        if( hr != DDERR_OUTOFVIDEOMEMORY )
            return D3DFWERR_NOBACKBUFFER;
        DEBUG_MSG( _T("Error: Out of video memory") );
        return DDERR_OUTOFVIDEOMEMORY;
    }

	ZeroMemory(&m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT));
	m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	hr = m_pddsBackBuffer->GetPixelFormat(&m_ddpfBackBufferPixelFormat);
	hr = m_pddsBackBuffer->GetSurfaceDesc( &ddsd );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreSurfaces()
// Desc: Checks for lost surfaces and restores them if lost. Note: Don't
//       restore render surface, since it's just a duplicate ptr.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::RestoreSurfaces()
{
	// Restore all surfaces (including video memory vertex buffers)
	m_pDD->RestoreAllSurfaces();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Move()
// Desc: Moves the screen rect for windowed renderers
//-----------------------------------------------------------------------------
VOID CD3DFramework7::Move( INT x, INT y )
{
    if( TRUE == m_bIsFullscreen )
        return;

    SetRect( &m_rcScreenRect, x, y, x + m_dwRenderWidth, y + m_dwRenderHeight );
}




//-----------------------------------------------------------------------------
// Name: FlipToGDISurface()
// Desc: Puts the GDI surface in front of the primary, so that dialog
//       boxes and other windows drawing funcs may happen.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::FlipToGDISurface( BOOL bDrawFrame )
{
    if( m_pDD && m_bIsFullscreen )
    {
        m_pDD->FlipToGDISurface();

        if( bDrawFrame )
        {
            DrawMenuBar( m_hWnd );
            RedrawWindow( m_hWnd, NULL, NULL, RDW_FRAME );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ShowFrame()
// Desc: Show the frame on the primary surface, via a blt or a flip.
//-----------------------------------------------------------------------------
HRESULT CD3DFramework7::ShowFrame()
{
    if( NULL == m_pddsFrontBuffer )
        return D3DFWERR_NOTINITIALIZED;

    if( m_bIsFullscreen )
    {
        // We are in fullscreen mode, so perform a flip.
        if( m_bIsStereo )
            return m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT | DDFLIP_STEREO );
        else
            return m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT );
    }
    else
    {
        // We are in windowed mode, so perform a blit.
        return m_pddsFrontBuffer->Blt( &m_rcScreenRect, m_pddsBackBuffer,
                                       NULL, DDBLT_WAIT, NULL );
    }
}



