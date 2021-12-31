//-----------------------------------------------------------------------------
// File: D3DUtil.h
//
// Desc: Helper functions and typing shortcuts for Direct3D programming.
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef D3DUTIL_H
#define D3DUTIL_H
#include <ddraw.h>
#include <d3d.h>




//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
const TCHAR* D3DUtil_GetDXSDKMediaPath();

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }




//-----------------------------------------------------------------------------
// Short cut functions for creating and using DX structures
//-----------------------------------------------------------------------------
VOID D3DUtil_InitDeviceDesc( D3DDEVICEDESC7& ddDevDesc );
VOID D3DUtil_InitSurfaceDesc( DDSURFACEDESC2& ddsd, DWORD dwFlags=0,
                              DWORD dwCaps=0 );

//-----------------------------------------------------------------------------
// Debug printing support
//-----------------------------------------------------------------------------

HRESULT _DbgOut( TCHAR*, DWORD, HRESULT, TCHAR* );

#if defined(DEBUG) | defined(_DEBUG)
    #define DEBUG_MSG(str)    _DbgOut( __FILE__, (DWORD)__LINE__, 0, str )
    #define DEBUG_ERR(hr,str) _DbgOut( __FILE__, (DWORD)__LINE__, hr, str )
#else
    #define DEBUG_MSG(str)    (0L)
    #define DEBUG_ERR(hr,str) (hr)
#endif




#endif // D3DUTIL_H
