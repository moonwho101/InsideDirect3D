
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
    HRESULT Render();

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
			
	HINSTANCE hInstApp;
};

#endif //__ROADRAGE_H