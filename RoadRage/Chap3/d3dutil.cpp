//-----------------------------------------------------------------------------
// File: D3DUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#define D3D_OVERLOADS
#define STRICT
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include "D3DUtil.h"




//-----------------------------------------------------------------------------
// Name: D3DUtil_GetDXSDKMediaPath()
// Desc: Returns the DirectX SDK media path, as stored in the system registry
//       during the SDK install.
//-----------------------------------------------------------------------------
const TCHAR* D3DUtil_GetDXSDKMediaPath()
{
    static TCHAR strNull[2] = _T("");
    static TCHAR strPath[512];
    HKEY  key;
    DWORD type, size = 512;

    // Open the appropriate registry key
    LONG result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Microsoft\\DirectX"),
                                0, KEY_READ, &key );
    if( ERROR_SUCCESS != result )
        return strNull;

    result = RegQueryValueEx( key, _T("DXSDK Samples Path"), NULL,
                              &type, (BYTE*)strPath, &size );
    RegCloseKey( key );

    if( ERROR_SUCCESS != result )
        return strNull;

    lstrcat( strPath, _T("\\D3DIM\\Media\\") );

    return strPath;
}




//-----------------------------------------------------------------------------
// Name: D3DUtil_InitSurfaceDesc()
// Desc: Helper function called to build a DDSURFACEDESC2 structure,
//       typically before calling CreateSurface() or GetSurfaceDesc()
//-----------------------------------------------------------------------------
VOID D3DUtil_InitSurfaceDesc( DDSURFACEDESC2& ddsd, DWORD dwFlags,
                              DWORD dwCaps )
{
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize                 = sizeof(ddsd);
    ddsd.dwFlags                = dwFlags;
    ddsd.ddsCaps.dwCaps         = dwCaps;
    ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
}


//-----------------------------------------------------------------------------
// Name: _DbgOut()
// Desc: Outputs a message to the debug stream
//-----------------------------------------------------------------------------
HRESULT _DbgOut( TCHAR* strFile, DWORD dwLine, HRESULT hr, TCHAR* strMsg )
{
    TCHAR buffer[256];
    wsprintf( buffer, _T("%s(%ld): "), strFile, dwLine );
    OutputDebugString( buffer );
    OutputDebugString( strMsg );

    if( hr )
    {
        wsprintf( buffer, _T("(hr=%08lx)\n"), hr );
        OutputDebugString( buffer );
    }

    OutputDebugString( _T("\n") );

    return hr;
}



