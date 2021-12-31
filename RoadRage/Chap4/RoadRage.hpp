
#ifndef __ROADRAGE_H
#define __ROADRAGE_H

#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "D3DRes.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------

class CMyD3DApplication : public CD3DApplication
{

    D3DLIGHTTYPE m_dltType;
    VOID    Cleanup3DEnvironment();
	
protected:
    HRESULT InitDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FinalCleanup();
	
public:
	
    CMyD3DApplication();

	BOOL	m_bBufferPaused;    
    BOOL    m_bWindowed;
    BOOL    m_bToggleFullscreen;
	BOOL	IsRenderingOk;
	void	DisplayCredits(HWND hwnd);
	void	DisplayRRStats(HWND hwnd);
	void	DisplayLegalInfo(HWND hwnd);

    VOID    OutputText( DWORD x, DWORD y, TCHAR* str );
	HRESULT Render3DEnvironment();

	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam );

	static HRESULT hr;

	D3DVALUE		angx, angy, angz; 
	D3DVALUE		look_up_ang;
    D3DVECTOR       m_vEyePt;
    D3DVECTOR       m_vLookatPt;
    D3DVECTOR       m_vUpVec;
    FLOAT           m_fEyeDistance; // Offset of each eye from m_vEyePt
    BOOL            m_bUseViewPoints;

	int current_mode;
	int toggle_delay ;
	int current_screen_status; // default, in case rrsetup.ini 
			
	int curr_seq;
	char szDevice_debug[256];
			
	D3DMATRIX trans, roty;
	D3DMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	D3DVALUE posx, posy, posz; //Player position
	D3DVECTOR from, at, up; // Used to set view matrix
	D3DVALUE Z_NEAR;
	D3DVALUE Z_FAR;
	D3DVALUE FOV; 
	D3DVALUE ASPECT_RATIO;
	int num_dp_commands_in_scene;

	DWORD intensity;
	BOOL bEnableLighting;
	D3DSHADEMODE ShadeMode;
	HINSTANCE hInstApp;
};

#endif //__ROADRAGE_H