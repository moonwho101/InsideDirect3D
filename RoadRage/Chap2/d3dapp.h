//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) 1998-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef  D3DAPP_H
#define  D3DAPP_H
#define  D3D_OVERLOADS
#include <d3d.h>
#include "D3DRes.h"



//-----------------------------------------------------------------------------
// Name: class CD3DApplication
// Desc:
//-----------------------------------------------------------------------------
class CD3DApplication
{
    // Internal variables and member functions
    BOOL            m_bActive;
    BOOL            m_bReady;

protected:
    HWND            m_hWnd;

    // Overridable variables for the app
    TCHAR*          m_strWindowTitle;

    // Overridable power management (APM) functions
    virtual LRESULT OnQuerySuspend( DWORD dwFlags );
    virtual LRESULT OnResumeSuspend( DWORD dwData );

public:
    // Functions to create, run, pause, and clean up the application
    virtual HRESULT Create( HINSTANCE, TCHAR* );
    virtual INT     Run();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Accessor functions
    HWND                 Get_hWnd()				{ return m_hWnd; };
	BOOL				 GetbActive()			{ return m_bActive; };
	BOOL				 GetbReady()			{ return m_bReady; };
	VOID				 SetbReady(BOOL val)	{ m_bReady = val; };
	VOID				 SetbActive(BOOL val)	{ m_bActive = val; };
    // Class constructor
    CD3DApplication();
};




#endif // D3DAPP_H


