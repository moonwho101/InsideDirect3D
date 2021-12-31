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
#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "D3DRes.h"




//-----------------------------------------------------------------------------
// Name: class CD3DApplication
// Desc:
//-----------------------------------------------------------------------------
class CD3DApplication
{
    // Internal variables and member functions
    CD3DFramework7* m_pFramework;
    BOOL            m_bActive;
    BOOL            m_bReady;

    BOOL            m_bFrameMoving;
    BOOL            m_bSingleStep;
    DWORD           m_dwBaseTime;
    DWORD           m_dwStopTime;

    HRESULT Initialize3DEnvironment();

    virtual HRESULT Render3DEnvironment();
    VOID    Cleanup3DEnvironment();
    VOID    DisplayFrameworkError( HRESULT, DWORD );

public:
	    HRESULT Change3DEnvironment();

protected:
    HWND                 m_hWnd;
    D3DEnum_DeviceInfo*  m_pDeviceInfo;
    LPDIRECTDRAW7        m_pDD;
    LPDIRECTDRAWSURFACE7 m_pddsRenderTarget;
    LPDIRECTDRAWSURFACE7 m_pddsRenderTargetLeft; // For stereo modes
    DDSURFACEDESC2       m_ddsdRenderTarget;

    // Overridable variables for the app
    TCHAR*               m_strWindowTitle;
    BOOL                 m_bAppUseStereo;
    BOOL                 m_bShowStats;
    HRESULT              (*m_fnConfirmDevice)(DDCAPS*, D3DDEVICEDESC7*);

    // Overridable functions for the 3D scene created by the app
    virtual HRESULT OneTimeSceneInit()     { return S_OK; }
    virtual HRESULT InitDeviceObjects()    { return S_OK; }
    virtual HRESULT DeleteDeviceObjects()  { return S_OK; }
    virtual HRESULT Render()               { return S_OK; }
    virtual HRESULT FrameMove( FLOAT )     { return S_OK; }
    virtual HRESULT RestoreSurfaces()      { return S_OK; }
    virtual HRESULT FinalCleanup()         { return S_OK; }

    // Overridable power management (APM) functions
    virtual LRESULT OnQuerySuspend( DWORD dwFlags );
    virtual LRESULT OnResumeSuspend( DWORD dwData );

    // View control functions (for stereo-enabled applications)
    D3DMATRIX m_matLeftView;
    D3DMATRIX m_matRightView;
    D3DMATRIX m_matView;
    VOID    SetAppLeftViewMatrix( D3DMATRIX mat )  { m_matLeftView  = mat; }
    VOID    SetAppRightViewMatrix( D3DMATRIX mat ) { m_matRightView = mat; }
    VOID    SetAppViewMatrix( D3DMATRIX mat )      { m_matView      = mat; }

    // Miscellaneous functions
    VOID    ShowStats();
    VOID    OutputText( DWORD x, DWORD y, TCHAR* str );

public:
    // Functions to create, run, pause, and clean up the application
    virtual HRESULT Create( HINSTANCE, TCHAR* );
    virtual INT     Run();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual VOID    Pause( BOOL bPause );

	// Accessor functions
	CD3DFramework7 *     GetFramework()			{ return m_pFramework; };
    HWND                 Get_hWnd()				{ return m_hWnd; };
	BOOL				 GetbActive()			{ return m_bActive; };
	BOOL				 GetbReady()			{ return m_bReady; };
	VOID				 SetbReady(BOOL val)	{ m_bReady = val; };
	VOID				 SetbActive(BOOL val)	{ m_bActive = val; };
	VOID				 SetbFrameMoving(BOOL val)	{ m_bFrameMoving = val; };
    BOOL				 GetbFrameMoving()		{ return m_bFrameMoving; };
    DWORD				 GetBaseTime()			{ return m_dwBaseTime; };
    DWORD				 GetStopTime()			{ return m_dwStopTime; };
	VOID				 SetBaseTime(DWORD val)	{ m_dwBaseTime = val; };
	VOID				 SetStopTime(DWORD val)	{ m_dwStopTime = val; };
    VOID				 DeleteFramework()		{ SAFE_DELETE( m_pFramework ); };
	BOOL				 GetSingleStep()		{ return m_bSingleStep; };
	VOID				 SetSingleStep(BOOL val) { m_bSingleStep = val; };
    // Class constructor
    CD3DApplication();
};




#endif // D3DAPP_H


